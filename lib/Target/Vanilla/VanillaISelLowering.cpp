//===-- VanillaISelLowering.cpp - Vanilla DAG Lowering Implementation -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that Vanilla uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#include "VanillaISelLowering.h"
#include "Vanilla.h"
#include "VanillaRegisterInfo.h"
#include "VanillaTargetMachine.h"
#include "VanillaSubtarget.h"

#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "vanilla-lower"

// Calling Convention Implementation
#include "VanillaGenCallingConv.inc"

VanillaTargetLowering::VanillaTargetLowering(const TargetMachine &TM,
                                     const VanillaSubtarget &STI)
    : TargetLowering(TM), Subtarget(&STI) {

  // Set up the register classes.
  addRegisterClass(MVT::i32, &Vanilla::GPRRegClass);
  // Compute derived properties from the register classes
  computeRegisterProperties(STI.getRegisterInfo());
      
  setStackPointerRegisterToSaveRestore(Vanilla::R3);
      
  setOperationAction(ISD::MUL, MVT::i32, Custom);

  setOperationAction(ISD::BR_CC, MVT::i32, Custom);
  setOperationAction(ISD::SELECT, MVT::i32, Expand);
  setOperationAction(ISD::SELECT_CC, MVT::i32, Custom);
      
  setOperationAction(ISD::GlobalAddress, MVT::i32, Custom);
}

SDValue VanillaTargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {
  default:
    llvm_unreachable("unimplemented operand");
    case ISD::BR_CC:
      return LowerBR_CC(Op,DAG);
    case ISD::GlobalAddress:
      return LowerGlobalAddress(Op, DAG);
    case ISD::MUL:
      return LowerMUL(Op, DAG);
    case ISD::SELECT_CC:
      return LowerSELECT_CC(Op, DAG);
  }
}

const char *VanillaTargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch ((VanillaISD::NodeType)Opcode) {
  case VanillaISD::FIRST_NUMBER:
    break;
  case VanillaISD::CALL:
    return "VanillaISD::CALL";
  case VanillaISD::SELECT_CC:
    return "VanillaISD::SELECT_CC";
  case VanillaISD::BR_CC:
    return "VanillaISD::BR_CC";
  case VanillaISD::Wrapper:
    return "VanillaISD::Wrapper";
  case VanillaISD::RET_FLAG:
    return "VanillaISA::RET_FLAG";
  }
  return nullptr;
}

MachineBasicBlock *
VanillaTargetLowering::EmitInstrWithCustomInserter(MachineInstr *MI,
                                               MachineBasicBlock *BB) const {
  if(MI->getOpcode() == Vanilla::Select){
    const TargetInstrInfo &TII = *BB->getParent()->getSubtarget().getInstrInfo();
    DebugLoc DL = MI->getDebugLoc();
    //DST= SELECT LHS, RHS, TargetCC, TrueV, FalseV
    // ->
    //thisMBB:
    //  ...
    //  MOV DST, FalseV
    //  MOV R1, LHS
    //  SUB R1, RHS
    //  BEQZ trueMBB
    //  BL  nextMBB
    //trueMBB:
    //  MOV DST,TrueV
    //  BL nextMBB
    //nextMBB:
    //
    const BasicBlock *LLVM_BB = BB->getBasicBlock();
    MachineFunction::iterator I = ++BB->getIterator();
    MachineFunction *F = BB->getParent();
    MachineBasicBlock *trueMBB = F->CreateMachineBasicBlock(LLVM_BB);
    MachineBasicBlock *nextMBB = F->CreateMachineBasicBlock(LLVM_BB);
    F->insert(I, trueMBB);
    F->insert(I, nextMBB);
    nextMBB->splice(nextMBB->begin(), BB, std::next(MachineBasicBlock::iterator(MI)), BB->end());
    nextMBB->transferSuccessorsAndUpdatePHIs(BB);
    BB->addSuccessor(trueMBB);
    BB->addSuccessor(nextMBB);
    trueMBB->addSuccessor(nextMBB);
    
    unsigned DST = MI->getOperand(0).getReg();
    unsigned LHS = MI->getOperand(1).getReg();
    unsigned RHS = MI->getOperand(2).getReg();
    int TargetCC = MI->getOperand(3).getImm();
    unsigned TrueV = MI->getOperand(4).getReg();
    unsigned FalseV = MI->getOperand(5).getReg();
    //unsigned newR=Vanilla::R1;
    unsigned newR=F->getRegInfo().createVirtualRegister(&Vanilla::GPRRegClass);
    BuildMI(BB, DL, TII.get(Vanilla::MOV)).addReg(DST).addReg(FalseV);
    BuildMI(BB, DL, TII.get(Vanilla::MOV)).addReg(newR).addReg(LHS);
    BuildMI(BB, DL, TII.get(Vanilla::SUBU)).addReg(newR).addReg(newR).addReg(RHS);
    switch (TargetCC) {
      case ISD::SETGT:
        llvm_unreachable("SETGT");
        break;
      case ISD::SETUGT:
        llvm_unreachable("SETUGT");
        break;
      case ISD::SETGE:
        llvm_unreachable("SETGE");
        break;
      case ISD::SETUGE:
        llvm_unreachable("SETUGE");
        break;
      case ISD::SETEQ:
        BuildMI(BB, DL, TII.get(Vanilla::BEQZ)).addReg(newR).addMBB(trueMBB);
        BuildMI(BB, DL, TII.get(Vanilla::BL)).addMBB(nextMBB);
        break;
      case ISD::SETNE:
        llvm_unreachable("SETNE");
        break;
      default:
        report_fatal_error("unimplemented select CondCode " + Twine(TargetCC));
    }
    MachineInstr *split_point=BuildMI(BB, DL, TII.get(Vanilla::MOV)).addReg(DST).addReg(TrueV);
    BuildMI(BB, DL, TII.get(Vanilla::BL)).addMBB(nextMBB);
    trueMBB->splice(trueMBB->begin(), BB, MachineBasicBlock::iterator(split_point),BB->end());
    BB=nextMBB;
    
    
    F->dump();
  }
  else{
    llvm_unreachable("unhandled custom inserted instruction.");
  }
  MI->eraseFromParent(); // The pseudo instruction is gone now.
  return BB;
}

SDValue VanillaTargetLowering::
LowerFormalArguments(SDValue Chain,
                     CallingConv::ID CallConv,
                     bool IsVarArg,
                     const SmallVectorImpl<ISD::InputArg> &Ins,
                     SDLoc DL,
                     SelectionDAG &DAG,
                     SmallVectorImpl<SDValue> &InVals) const {
  switch (CallConv) {
    default:
      llvm_unreachable("Unsupported calling convention");
    case CallingConv::C:
    case CallingConv::Fast:
      break;
  }
  
  MachineFunction &MF = DAG.getMachineFunction();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();
  
  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, CC_Vanilla);
  
  for (auto &VA : ArgLocs) {
    if (VA.isRegLoc()) {
      // Arguments passed in registers
      EVT RegVT = VA.getLocVT();
      switch (RegVT.getSimpleVT().SimpleTy) {
        default: {
          errs() << "LowerFormalArguments Unhandled argument type: "
          << RegVT.getSimpleVT().SimpleTy << '\n';
          llvm_unreachable(0);
        }
        case MVT::i32:
          unsigned VReg = RegInfo.createVirtualRegister(&Vanilla::GPRRegClass);
          RegInfo.addLiveIn(VA.getLocReg(), VReg);
          SDValue ArgValue = DAG.getCopyFromReg(Chain, DL, VReg, RegVT);
          
          // If this is an 8/16/32-bit value, it is really passed promoted to 32
          // bits. Insert an assert[sz]ext to capture this, then truncate to the
          // right size.
          if (VA.getLocInfo() == CCValAssign::SExt)
            ArgValue = DAG.getNode(ISD::AssertSext, DL, RegVT, ArgValue,
                                   DAG.getValueType(VA.getValVT()));
          else if (VA.getLocInfo() == CCValAssign::ZExt)
            ArgValue = DAG.getNode(ISD::AssertZext, DL, RegVT, ArgValue,
                                   DAG.getValueType(VA.getValVT()));
          
          if (VA.getLocInfo() != CCValAssign::Full)
            ArgValue = DAG.getNode(ISD::TRUNCATE, DL, VA.getValVT(), ArgValue);
          
          InVals.push_back(ArgValue);
      }
    } else {
      llvm_unreachable("Trying to pass too many arguments to a function.\n");
    }
  }
  
  if (IsVarArg || MF.getFunction()->hasStructRetAttr()) {
    llvm_unreachable("functions with VarArgs or StructRet are not supported");
  }
  
  return Chain;
}

SDValue
VanillaTargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                               bool IsVarArg,
                               const SmallVectorImpl<ISD::OutputArg> &Outs,
                               const SmallVectorImpl<SDValue> &OutVals,
                               SDLoc DL, SelectionDAG &DAG) const {
  
  // CCValAssign - represent the assignment of the return value to a location
  SmallVector<CCValAssign, 16> RVLocs;
  MachineFunction &MF = DAG.getMachineFunction();
  
  // CCState - Info about the registers and stack slot.
  CCState CCInfo(CallConv, IsVarArg, MF, RVLocs, *DAG.getContext());
  
  if (MF.getFunction()->getReturnType()->isAggregateType()) {
    llvm_unreachable("aggregate type return unsupported\n");
  }
  
  // Analize return values.
  CCInfo.AnalyzeReturn(Outs, RetCC_Vanilla);
  
  SDValue Flag;
  SmallVector<SDValue, 4> RetOps(1, Chain);
  
  // Copy the result values into the output registers.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");
    
    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), OutVals[i], Flag);
    
    // Guarantee that all emitted copies are stuck together,
    // avoiding something bad.
    Flag = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }
  
  unsigned Opc = VanillaISD::RET_FLAG;
  RetOps[0] = Chain; // Update chain.
  
  // Add the flag if we have it.
  if (Flag.getNode())
    RetOps.push_back(Flag);
  
  return DAG.getNode(Opc, DL, MVT::Other, RetOps);
}

SDValue VanillaTargetLowering::LowerBR_CC(SDValue Op, SelectionDAG &DAG) const {
  SDValue Chain = Op.getOperand(0);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(1))->get();
  SDValue LHS = Op.getOperand(2);
  SDValue RHS = Op.getOperand(3);
  SDValue Dest = Op.getOperand(4);
  SDLoc DL(Op);
  
  SDValue SUB = DAG.getNode(ISD::SUB, DL, MVT::i32, LHS, RHS);
  if(CC==ISD::SETGE){
    CC=ISD::SETGT;
    SUB=DAG.getNode(ISD::ADD, DL, MVT::i32, SUB, DAG.getConstant(1,DL, MVT::i32));
  }
  else if(CC==ISD::SETLE){
    llvm_unreachable("haven't implemented SETLE yet.");
  }
  
  return DAG.getNode(VanillaISD::BR_CC, DL, Op.getValueType(), Chain, SUB,
                     DAG.getConstant(CC, DL, MVT::i32), Dest);
}

SDValue VanillaTargetLowering::LowerSELECT_CC(SDValue Op, SelectionDAG &DAG) const {
  SDValue LHS = Op.getOperand(0);
  SDValue RHS = Op.getOperand(1);
  SDValue TrueV = Op.getOperand(2);
  SDValue FalseV = Op.getOperand(3);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(4))->get();
  SDLoc DL(Op);
  
  SDValue TargetCC = DAG.getConstant(CC, DL, MVT::i32);
  
  SDVTList VTs = DAG.getVTList(Op.getValueType(), MVT::Glue);
  SDValue Ops[] = {LHS, RHS, TargetCC, TrueV, FalseV};
  return DAG.getNode(VanillaISD::SELECT_CC, DL, VTs, Ops);
}

SDValue VanillaTargetLowering::LowerMUL(SDValue Op, SelectionDAG &DAG) const {
  SDValue LHS = Op.getOperand(0);
  SDValue RHS = Op.getOperand(1);
  SDLoc DL(Op);
  
  if (RHS.getOpcode()==ISD::Constant){
    uint64_t cons=Op.getConstantOperandVal(1);
    if (cons>>63==1){
      llvm_unreachable("negative constant unhandled.");
    }
    //# mathematically, not efficient enough.
    /*
#define square_dim 10
    int square2[square_dim];
    square2[0]=1;
    for(int i=1;i<square_dim;i++){
      square2[i]=2*square2[i-1];
      errs()<<square2[i]<<"\n";
    }
     */
    //# logically, bitwise multiply.
    // res=0
    // shift_amount=0
    // while(cons!=0)
    //   lastbit=cons & 1;
    //   if(lastbit==1)
    //     res+=LHS<<shift_amount
    //   shift_amount++
    //   cons=cons>>1;
    SDValue RES=DAG.getConstant(0, DL, MVT::i32);
    int shift_mount=0;
    //errs()<<"Const to multiply:"<<cons<<"\n";
    while (cons!=0){
      if((cons&1) ==1){
        if(shift_mount!=0){
          SDValue NEW_LHS=DAG.getNode(ISD::SHL, DL, MVT::i32, LHS, DAG.getConstant(shift_mount, DL, MVT::i32));
          RES=DAG.getNode(ISD::ADD, DL, MVT::i32, RES, NEW_LHS);
        }
        else{
          RES=DAG.getNode(ISD::ADD, DL, MVT::i32, RES, LHS);
        }
      }
      shift_mount++;
      cons=cons>>1;
    }
    //RES->dumpr();
    return RES;
  }
  else{
    llvm_unreachable("unhandled multiplication.");
  }
}

SDValue VanillaTargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
                                     SmallVectorImpl<SDValue> &InVals) const {
  SelectionDAG &DAG = CLI.DAG;
  auto &Outs = CLI.Outs;
  auto &OutVals = CLI.OutVals;
  auto &Ins = CLI.Ins;
  SDValue Chain = CLI.Chain;
  SDValue Callee = CLI.Callee;
  bool &IsTailCall = CLI.IsTailCall;
  CallingConv::ID CallConv = CLI.CallConv;
  bool IsVarArg = CLI.IsVarArg;
  MachineFunction &MF = DAG.getMachineFunction();
  
  // Vanilla target does not support tail call optimization.
  IsTailCall = false;
  
  switch (CallConv) {
    default:
      report_fatal_error("Unsupported calling convention");
    case CallingConv::Fast:
    case CallingConv::C:
      break;
  }
  
  // Analyze operands of the call, assigning locations to each operand.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());
  
  CCInfo.AnalyzeCallOperands(Outs, CC_Vanilla);
  
  unsigned NumBytes = CCInfo.getNextStackOffset();
  
  if (Outs.size() > 2) {
    llvm_unreachable("passing too many out-going parameters.");
  }
  
  for (auto &Arg : Outs) {
    ISD::ArgFlagsTy Flags = Arg.Flags;
    if (!Flags.isByVal())
      continue;
    llvm_unreachable("Struct passed by value is not supported.");
  }
  
  auto PtrVT = getPointerTy(MF.getDataLayout());
  Chain = DAG.getCALLSEQ_START(
                               Chain, DAG.getConstant(NumBytes, CLI.DL, PtrVT, true), CLI.DL);
  
  SmallVector<std::pair<unsigned, SDValue>, 5> RegsToPass;
  
  // Walk arg assignments
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    SDValue Arg = OutVals[i];
    
    // Promote the value if needed.
    switch (VA.getLocInfo()) {
      default:
        llvm_unreachable("Unknown loc info");
      case CCValAssign::Full:
        break;
      case CCValAssign::SExt:
        Arg = DAG.getNode(ISD::SIGN_EXTEND, CLI.DL, VA.getLocVT(), Arg);
        break;
      case CCValAssign::ZExt:
        Arg = DAG.getNode(ISD::ZERO_EXTEND, CLI.DL, VA.getLocVT(), Arg);
        break;
      case CCValAssign::AExt:
        Arg = DAG.getNode(ISD::ANY_EXTEND, CLI.DL, VA.getLocVT(), Arg);
        break;
    }
    
    // Push arguments into RegsToPass vector
    if (VA.isRegLoc())
      RegsToPass.push_back(std::make_pair(VA.getLocReg(), Arg));
    else
      llvm_unreachable("call arg pass bug");
  }
  
  SDValue InFlag;
  
  // Build a sequence of copy-to-reg nodes chained together with token chain and
  // flag operands which copy the outgoing args into registers.  The InFlag in
  // necessary since all emitted instructions must be stuck together.
  for (auto &Reg : RegsToPass) {
    Chain = DAG.getCopyToReg(Chain, CLI.DL, Reg.first, Reg.second, InFlag);
    InFlag = Chain.getValue(1);
  }
  
  // If the callee is a GlobalAddress node (quite common, every direct call is)
  // turn it into a TargetGlobalAddress node so that legalize doesn't hack it.
  // Likewise ExternalSymbol -> TargetExternalSymbol.
  if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee))
    Callee = DAG.getTargetGlobalAddress(G->getGlobal(), CLI.DL, PtrVT,
                                        G->getOffset(), 0);
  else if (ExternalSymbolSDNode *E = dyn_cast<ExternalSymbolSDNode>(Callee))
    Callee = DAG.getTargetExternalSymbol(E->getSymbol(), PtrVT, 0);
  
  // Returns a chain & a flag for retval copy to use.
  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);
  SmallVector<SDValue, 8> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);
  
  // Add argument registers to the end of the list so that they are
  // known live into the call.
  for (auto &Reg : RegsToPass)
    Ops.push_back(DAG.getRegister(Reg.first, Reg.second.getValueType()));
  
  // Add a register mask operand representing the call-preserved registers.
  const TargetRegisterInfo *TRI = Subtarget->getRegisterInfo();
  const uint32_t *Mask = TRI->getCallPreservedMask(DAG.getMachineFunction(), CLI.CallConv);
  assert(Mask && "Missing call preserved mask for calling convention");
  Ops.push_back(DAG.getRegisterMask(Mask));
  
  if (InFlag.getNode())
    Ops.push_back(InFlag);
  
  Chain = DAG.getNode(VanillaISD::CALL, CLI.DL, NodeTys, Ops);
  InFlag = Chain.getValue(1);
  
  // Create the CALLSEQ_END node.
  Chain = DAG.getCALLSEQ_END(
                             Chain, DAG.getConstant(NumBytes, CLI.DL, PtrVT, true),
                             DAG.getConstant(0, CLI.DL, PtrVT, true), InFlag, CLI.DL);
  InFlag = Chain.getValue(1);
  
  // Handle result values, copying them out of physregs into vregs that we
  // return.
  return LowerCallResult(Chain, InFlag, CallConv, IsVarArg, Ins, CLI.DL, DAG,
                         InVals);
}

SDValue VanillaTargetLowering::LowerCallResult(SDValue Chain, SDValue InFlag, CallingConv::ID CallConv, bool IsVarArg,
                                           const SmallVectorImpl<ISD::InputArg> &Ins, SDLoc DL, SelectionDAG &DAG,
                                           SmallVectorImpl<SDValue> &InVals) const {
  
  MachineFunction &MF = DAG.getMachineFunction();
  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, RVLocs, *DAG.getContext());
  
  if (Ins.size() >= 2) {
    llvm_unreachable("returning arguments with more than 2 registers.");
  }
  
  CCInfo.AnalyzeCallResult(Ins, RetCC_Vanilla);
  
  // Copy all of the result registers out of their specified physreg.
  for (auto &Val : RVLocs) {
    Chain = DAG.getCopyFromReg(Chain, DL, Val.getLocReg(),
                               Val.getValVT(), InFlag).getValue(1);
    InFlag = Chain.getValue(2);
    InVals.push_back(Chain.getValue(0));
  }
  
  return Chain;
}

SDValue VanillaTargetLowering::LowerGlobalAddress(SDValue Op,
                                              SelectionDAG &DAG) const {
  SDLoc DL(Op);
  const GlobalValue *GV = cast<GlobalAddressSDNode>(Op)->getGlobal();
  SDValue GA = DAG.getTargetGlobalAddress(GV, DL, MVT::i32);
  
  return DAG.getNode(VanillaISD::Wrapper, DL, MVT::i32, GA);
}
