//===-- VanillaInstrInfo.td - Target Description for Vanilla Target -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the Vanilla instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Instruction format superclass
//===----------------------------------------------------------------------===//

include "VanillaInstrFormats.td"

//===----------------------------------------------------------------------===//
// Instruction Pattern Stuff
//===----------------------------------------------------------------------===//

// These are target-independent nodes, but have target-specific formats.
def SDT_VanillaCallSeqStart : SDCallSeqStart<[SDTCisVT<0, iPTR>]>;
def SDT_VanillaCallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, iPTR>, SDTCisVT<1, iPTR>]>;
def SDT_VanillaCall    : SDTypeProfile<0, -1, [SDTCisVT<0, i32>]>;
//def SDT_VanillaRet     : SDTypeProfile<0, 1, [SDTCisVT<0, i32>]>;
def SDT_VanillaSelectCC     : SDTypeProfile<1, 5, [SDTCisSameAs<1, 2>, SDTCisSameAs<0, 4>, SDTCisSameAs<4, 5>]>;
def SDT_VanillaBrCC         : SDTypeProfile<0, 3, [SDTCisSameAs<0, 1>, SDTCisVT<2, OtherVT>]>;
def SDT_VanillaWrapper      : SDTypeProfile<1, 1, [SDTCisSameAs<0, 1>, SDTCisPtrTy<0>]>;

def Vanillacallseq_start: SDNode<"ISD::CALLSEQ_START", SDT_VanillaCallSeqStart,
                             [SDNPHasChain, SDNPOutGlue]>;
def Vanillacallseq_end  : SDNode<"ISD::CALLSEQ_END",   SDT_VanillaCallSeqEnd,
                             [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;
def Vanillacall         : SDNode<"VanillaISD::CALL", SDT_VanillaCall,
                             [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue,
                              SDNPVariadic]>;
def Vanillaretflag      : SDNode<"VanillaISD::RET_FLAG", SDTNone, [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;
def Vanillabrcc         : SDNode<"VanillaISD::BR_CC", SDT_VanillaBrCC, [SDNPHasChain, SDNPOutGlue, SDNPInGlue]>;
def Vanillaselectcc     : SDNode<"VanillaISD::SELECT_CC", SDT_VanillaSelectCC, [SDNPInGlue]>;
def Vanillawrapper      : SDNode<"VanillaISD::Wrapper", SDT_VanillaWrapper>;

def brtarget : Operand<OtherVT>;
def calltarget : Operand<i32>;

def simm11  : PatLeaf<(imm), [{ return isInt<11>(N->getSExtValue()); }]>;

def ADDRr : ComplexPattern<i32, 1, "SelectAddr", [frameindex], []>;

// Address operands
def MEMr : Operand<i32> {
    let PrintMethod = "printMemOperand";
    let MIOperandInfo = (ops GPR);
}

// Immediate value operands
def simm11Op : Operand<i32> {
//  let PrintMethod = "printImmOperand";
}

// Conditional code predicates - used for pattern matching for jump instructions
def Vanilla_CC_EQ  : PatLeaf<(imm), [{return (N->getZExtValue() == ISD::SETEQ);}]>;
def Vanilla_CC_NE  : PatLeaf<(imm), [{return (N->getZExtValue() == ISD::SETNE);}]>;
def Vanilla_CC_GT  : PatLeaf<(imm), [{return (N->getZExtValue() == ISD::SETGT);}]>;
def Vanilla_CC_LT  : PatLeaf<(imm), [{return (N->getZExtValue() == ISD::SETLT);}]>;

// ADJCALLSTACKDOWN/UP pseudo insns
def ADJCALLSTACKDOWN : Pseudo<(outs), (ins i32imm:$amt),
                              "#ADJCALLSTACKDOWN $amt",
                              [(Vanillacallseq_start timm:$amt)]>;
def ADJCALLSTACKUP   : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                              "#ADJCALLSTACKUP $amt1 $amt2",
                              [(Vanillacallseq_end timm:$amt1, timm:$amt2)]>;

// LOAD instructions
class LOAD<bits<5> Op, string OpcodeStr, list<dag> Pattern>
: InstVanilla<(outs GPR:$rd), (ins MEMr:$rs),!strconcat(OpcodeStr, "\t$rd, [$rs]"), Pattern> {
    bits<5> rd;
    bits<6> rs;
    let Opcode=Op;
    let Inst{10-6} = rd;
    let Inst{5-0} = rs;
}

// STORE instructions
class STORE<bits<5> Op, string OpcodeStr, list<dag> Pattern>
: InstVanilla<(outs), (ins GPR:$rs, MEMr:$rd), !strconcat(OpcodeStr, "\t[$rd], $rs"), Pattern> {
    bits<5> rd;
    bits<6> rs;
    let Opcode=Op;
    let Inst{10-6} = rd;
    let Inst{5-0} = rs;
}

class REG1<bits<5> Op, string OpcodeStr, list<dag> Pattern>
: InstVanilla<(outs), (ins GPR:$rs), !strconcat(OpcodeStr, "\t$rs"), Pattern> {
    bits<6> rs;
    
    let Opcode=Op;
    let Inst{10-6}=0;
    let Inst{5-0}=rs;
}

class REG2<bits<5> Op, string OpcodeStr, list<dag> Pattern>
: InstVanilla<(outs GPR:$rd), (ins GPR:$rs), !strconcat(OpcodeStr, "\t$rd, $rs"), Pattern> {
  bits<5> rd;
  bits<6> rs;
  
  let Opcode=Op;
  let Inst{10-6}=rd;
  let Inst{5-0}=rs;
}

class REG2_1<bits<5> Op, string OpcodeStr, list<dag> Pattern>
: InstVanilla<(outs GPR:$rd), (ins GPR:$rds, GPR:$rs), !strconcat(OpcodeStr, "\t$rd, $rds, $rs"), Pattern> {
  bits<5> rd;
  bits<6> rs;
  bits<5> rds;
  
  let Opcode=Op;
  let Inst{10-6}=rd;
  let Inst{5-0}=rs;
  
  let TwoOperandAliasConstraint="$rd=$rds";
}

class REG1_BRANCH11<bits<5> Op, string OpcodeStr, PatLeaf Cond>
: InstVanilla<(outs), (ins GPR:$rd, brtarget:$BrDst),
              !strconcat(OpcodeStr, "\t$rd, 0, goto $BrDst"), [(Vanillabrcc i32:$rd, Cond, bb:$BrDst)]> {
  //bits<5> rd;
  bits<11> BrDst;
  
  let Opcode=Op;
  //let Inst{10-6}=rd;
  let Inst{10-0}=BrDst;
}

class BRANCH11<bits<5> Op, string OpcodeStr, list<dag> Pattern>
: InstVanilla<(outs), (ins brtarget:$BrDst), !strconcat(OpcodeStr, "\t$BrDst"), Pattern>{
  bits<11> BrDst;
  
  let Opcode=Op;
  let Inst{10-0}=BrDst;
}

class REG1_OFFSET11<bits<5> Op, string OpcodeStr, list<dag> Pattern>
: InstVanilla<(outs GPR:$dstReg), (ins simm11Op:$offset), !strconcat(OpcodeStr, "\t$dstReg, $offset"), Pattern>{
  bits<11> offset;
  
  let Opcode=Op;
  let Inst{10-0}=offset;
}

class REG2_OFFSET11<bits<5> Op, string OpcodeStr, list<dag> Pattern>
: InstVanilla<(outs GPR:$dstReg), (ins GPR:$srcReg, simm11Op:$offset), !strconcat(OpcodeStr, "\t$dstReg, $srcReg, $offset"), Pattern>{
  bits<11> offset;
  
  let Opcode=Op;
  let Inst{10-0}=offset;
}

// Arithmatic
def ADDU:REG2_1<0x00,"addu", [(set i32:$rd, (add i32:$rds, i32:$rs))]>;
def SUBU:REG2_1<0x01,"subu", [(set i32:$rd, (sub i32:$rds, i32:$rs))]>;
def SLLV:REG2_1<0x02,"sllv", [(set i32:$rd, (shl i32:$rds, i32:$rs))]>;
def SRAV:REG2_1<0x03,"srav", [(set i32:$rd, (sra i32:$rds, i32:$rs))]>;
def SRLV:REG2_1<0x04,"srlv", [(set i32:$rd, (srl i32:$rds, i32:$rs))]>;
def AND :REG2_1<0x05,"and", [(set i32:$rd, (and i32:$rds, i32:$rs))]>;
def OR  :REG2_1<0x06,"or",  [(set i32:$rd, (or i32:$rds, i32:$rs))]>;
def NOR :REG2_1<0x07,"nor", []>;
def SLT :REG2_1<0x08,"slt", [(set i32:$rd, (setlt i32:$rds, i32:$rs))]>;
def SLTU:REG2_1<0x09,"sltu", []>;
def MULT:REG2_1<0x0b,"mult", [(set i32:$rd, (mul i32:$rds, i32:$rs))]>;
def SDIV:REG2_1<0x0d,"sdiv", [(set i32:$rd, (sdiv i32:$rds, i32:$rs))]>;

//def LBU: LOAD<0x19, "lbu", [(set i32:$rd,  (zextloadi8 ADDRr:$rs))]>;
def LBU: LOAD<0x19, "lbu", []>;
def LW : LOAD<0x18, "lw", [(set i32:$rd,  (load ADDRr:$rs))]>;
def LG : LOAD<0x1c, "lg", []>;
//def SB : STORE<0x1b, "sb", [(truncstorei8 i32:$rs, ADDRr:$rd)]>;
def SB : STORE<0x1b, "sb", []>;
def SW : STORE<0x1a, "sw", [(store i32:$rs, ADDRr:$rd)]>;

// call
let isCall=1, hasDelaySlot=0, Uses = [R0, R3], Defs = [R2, R5, R6, R7, R8] in {
  def CALL: Pseudo<(outs),(ins calltarget:$BrDst),"_call",[]>;
}
def : Pat<(Vanillacall tglobaladdr:$dst), (CALL tglobaladdr:$dst)>;

// Unconditional branch
let isBranch = 1, isTerminator = 1, hasDelaySlot=0, isBarrier = 1 in {
  def BL : BRANCH11<0x16, "bl", [(br bb:$BrDst)]>;
  def JAL : BRANCH11<0x15, "jal", []>;
}

// Conditonal branch
let isBranch = 1, isTerminator = 1, hasDelaySlot=0 in {
  def BEQZ: REG1_BRANCH11<0x10, "beqz",  Vanilla_CC_EQ>;
  def BNEQZ:REG1_BRANCH11<0x11, "bneqz", Vanilla_CC_NE>;
  def BGTZ :REG1_BRANCH11<0x12, "bgtz",  Vanilla_CC_GT>;
  def BLTZ :REG1_BRANCH11<0x13, "bltz",  Vanilla_CC_LT>;
}

// Return and indirect branch
def JALR:REG1<0x17,"jalr", [(brind i32:$rs)]>;

let isReturn = 1, isTerminator = 1, hasDelaySlot=0, isBarrier = 1,
    isNotDuplicable = 1 in {
  def RET: Pseudo<(outs),(ins),"_ret",[(Vanillaretflag)]>;
}
//Other
def MOV :REG2<0x0a,"mov",[]>;
def WAIT:REG2<0x0c,"wait",[]>;
def MOVI:REG1_OFFSET11<0x1d,"movi", [(set i32:$dstReg, simm11:$offset)]>;
def APPI:REG2_OFFSET11<0x1e,"appi", []>;

//hi/mi/lo
def LO10 : SDNodeXForm<imm, [{
  return CurDAG->getTargetConstant((unsigned)N->getSExtValue() & 0x3ff, SDLoc(N), MVT::i32);
}]>;

def MI10 : SDNodeXForm<imm, [{
  return CurDAG->getTargetConstant(((unsigned)N->getSExtValue() >> 10) &0x3ff, SDLoc(N), MVT::i32);
}]>;

def HI10 : SDNodeXForm<imm, [{
  return CurDAG->getTargetConstant(((unsigned)N->getSExtValue() >> 20) &0x3ff, SDLoc(N), MVT::i32);
}]>;

def TOP2 : SDNodeXForm<imm, [{
  return CurDAG->getTargetConstant(((unsigned)N->getSExtValue() >> 30) &0x3, SDLoc(N), MVT::i32);
}]>;

// Small immediates.
def : Pat<(i32 simm11:$val), (MOVI imm:$val)>;
// Arbitrary immediates.
//def : Pat<(i32 imm:$val), (APPI (MOVI (MI10 imm:$val)), (LO10 imm:$val))>;
def : Pat<(i32 imm:$val), (APPI (APPI (APPI (MOVI (TOP2 imm:$val)), (HI10 imm:$val)), (MI10 imm:$val)), (LO10 imm:$val))>;

//load a global address into register.
def : Pat<(Vanillawrapper tglobaladdr:$in), (MOVI tglobaladdr:$in)>;

// Select instructions.
let usesCustomInserter = 1 in {
  def Select : Pseudo<(outs GPR:$dst), (ins GPR:$lhs, GPR:$rhs, i32imm:$imm, GPR:$src, GPR:$src2),
                      "# Select PSEUDO $dst = $lhs $imm $rhs ? $src : $src2",
                      [(set i32:$dst,
                      (Vanillaselectcc i32:$lhs, i32:$rhs, (i32 imm:$imm), i32:$src, i32:$src2))]>;
}
