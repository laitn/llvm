//===-- VanillaISelLowering.h - Vanilla DAG Lowering Interface ----------*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_VANILLA_VANILLAISELLOWERING_H
#define LLVM_LIB_TARGET_VANILLA_VANILLAISELLOWERING_H

#include "Vanilla.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/Target/TargetLowering.h"

namespace llvm {
class VanillaSubtarget;
namespace VanillaISD {
enum NodeType : unsigned {
    FIRST_NUMBER = ISD::BUILTIN_OP_END,
    RET_FLAG,
    CALL,
    SELECT_CC,
    BR_CC,
    Wrapper
};
}

class VanillaTargetLowering : public TargetLowering {
   const VanillaSubtarget *Subtarget;
public:
  explicit VanillaTargetLowering(const TargetMachine &TM, const VanillaSubtarget &STI);

  // Provide custom lowering hooks for some operations.
  SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

  // This method returns the name of a target specific DAG node.
  const char *getTargetNodeName(unsigned Opcode) const override;

  MachineBasicBlock *
  EmitInstrWithCustomInserter(MachineInstr *MI,
                              MachineBasicBlock *BB) const override;
  SDValue
  LowerFormalArguments(SDValue Chain,
                       CallingConv::ID CallConv,
                       bool isVarArg,
                       const SmallVectorImpl<ISD::InputArg> &Ins,
                       SDLoc dl, SelectionDAG &DAG,
                       SmallVectorImpl<SDValue> &InVals) const override;
  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals, SDLoc DL,
                      SelectionDAG &DAG) const override;
  SDValue LowerCall(TargetLowering::CallLoweringInfo &CLI,
                    SmallVectorImpl<SDValue> &InVals) const override;
  
  // Lower the result values of a call, copying them out of physregs into vregs
  SDValue LowerCallResult(SDValue Chain, SDValue InFlag,
                          CallingConv::ID CallConv, bool IsVarArg,
                          const SmallVectorImpl<ISD::InputArg> &Ins, SDLoc DL,
                          SelectionDAG &DAG,
                          SmallVectorImpl<SDValue> &InVals) const;
  SDValue LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;
  
private:
  SDValue LowerBR_CC(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerMUL(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerSELECT(SDValue Op, SelectionDAG &DAG) const;
};
}

#endif

