//===- VanillaDisassembler.cpp - Disassembler for Vanilla -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is part of the Vanilla Disassembler.
//
//===----------------------------------------------------------------------===//

#include "Vanilla.h"
#include "VanillaRegisterInfo.h"
#include "VanillaSubtarget.h"
#include "llvm/MC/MCDisassembler.h"
#include "llvm/MC/MCFixedLenDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "vanilla-disassembler"

typedef MCDisassembler::DecodeStatus DecodeStatus;

namespace {

/// A disassembler class for Vanilla.
class VanillaDisassembler : public MCDisassembler {
public:
  VanillaDisassembler(const MCSubtargetInfo &STI, MCContext &Ctx)
      : MCDisassembler(STI, Ctx) {}
  virtual ~VanillaDisassembler() {}

  DecodeStatus getInstruction(MCInst &Instr, uint64_t &Size,
                              ArrayRef<uint8_t> Bytes, uint64_t Address,
                              raw_ostream &VStream,
                              raw_ostream &CStream) const override;
};
}

namespace llvm {
  extern Target TheVanillaTarget;
}

static MCDisassembler *createVanillaDisassembler(const Target &T,
                                                 const MCSubtargetInfo &STI,
                                                 MCContext &Ctx) {
  return new VanillaDisassembler(STI, Ctx);
}


extern "C" void LLVMInitializeVanillaDisassembler() {
  // Register the disassembler.
  TargetRegistry::RegisterMCDisassembler(TheVanillaTarget,
                                         createVanillaDisassembler);
}

static const unsigned GPRDecoderTable[] = {
  Vanilla::R0,Vanilla::R1,Vanilla::R2,Vanilla::R3,Vanilla::R4,
  Vanilla::R5,Vanilla::R6,Vanilla::R7,Vanilla::R8,Vanilla::R9,
  Vanilla::R10,Vanilla::R11,Vanilla::R12,Vanilla::R13,Vanilla::R14,
  Vanilla::R15,Vanilla::R16,Vanilla::R17,Vanilla::R18,Vanilla::R19,
  Vanilla::R20,Vanilla::R21,Vanilla::R22,Vanilla::R23,Vanilla::R24,
  Vanilla::R25,Vanilla::R26,Vanilla::R27,Vanilla::R28,Vanilla::R29,
  Vanilla::R30,Vanilla::R31
};
static DecodeStatus DecodeGPRRegisterClass(MCInst &Inst,
                                               unsigned RegNo,
                                               uint64_t Address,
                                               const void *Decoder) {
  if (RegNo > 31)
    return MCDisassembler::Fail;
  unsigned Reg = GPRDecoderTable[RegNo];
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

#include "VanillaGenDisassemblerTables.inc"

/// Read four bytes from the ArrayRef and return 32 bit word.
static DecodeStatus readInstruction16(ArrayRef<uint8_t> Bytes, uint64_t Address,
                                      uint64_t &Size, uint32_t &Insn,
                                      bool IsLittleEndian) {
  // We want to read exactly 4 Bytes of data.
  if (Bytes.size() < 2) {
    Size = 0;
    return MCDisassembler::Fail;
  }

  Insn = IsLittleEndian
  ? (Bytes[0] << 0) | (Bytes[1] << 8)
  : (Bytes[1] << 0) | (Bytes[0] << 8);
  
  return MCDisassembler::Success;
}

DecodeStatus VanillaDisassembler::getInstruction(MCInst &Instr, uint64_t &Size,
                                               ArrayRef<uint8_t> Bytes,
                                               uint64_t Address,
                                               raw_ostream &VStream,
                                               raw_ostream &CStream) const {
  uint32_t Insn;
  bool isLittleEndian = getContext().getAsmInfo()->isLittleEndian();
  DecodeStatus Result =
  readInstruction16(Bytes, Address, Size, Insn, isLittleEndian);
  if (Result == MCDisassembler::Fail)
    return MCDisassembler::Fail;
  
  // Calling the auto-generated decoder function.
  Result =
  decodeInstruction(DecoderTableVanilla16, Instr, Insn, Address, this, STI);
  
  if (Result != MCDisassembler::Fail) {
    Size = 2;
    return Result;
  }
  
  return MCDisassembler::Fail;
}
