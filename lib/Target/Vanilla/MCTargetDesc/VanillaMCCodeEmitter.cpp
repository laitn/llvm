//===-- VanillaMCCodeEmitter.cpp - Convert Vanilla code to machine code -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the VaniMCCodeEmitter class.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/VanillaMCTargetDesc.h"
#include "MCTargetDesc/VanillaFixupKinds.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define DEBUG_TYPE "mccodeemitter"

namespace {
class VanillaMCCodeEmitter : public MCCodeEmitter {
  VanillaMCCodeEmitter(const VanillaMCCodeEmitter &) = delete;
  void operator=(const VanillaMCCodeEmitter &) = delete;
  const MCRegisterInfo &MRI;
  bool IsLittleEndian;

public:
  VanillaMCCodeEmitter(const MCRegisterInfo &mri, bool IsLittleEndian)
    : MRI(mri), IsLittleEndian(IsLittleEndian) {}

  ~VanillaMCCodeEmitter() {}
  
  void encodeInstruction (const MCInst &Inst, raw_ostream &OS, SmallVectorImpl< MCFixup > &Fixups, const MCSubtargetInfo &STI)  const override;

  // getBinaryCodeForInstr - TableGen'erated function for getting the
  // binary encoding for an instruction.
  uint64_t getBinaryCodeForInstr(const MCInst &MI,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;

  // getMachineOpValue - Return binary encoding of operand. If the machin
  // operand requires relocation, record the relocation and return zero.
  unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;

};
}

MCCodeEmitter *llvm::createVanillaMCCodeEmitter(const MCInstrInfo &MCII,
                                            const MCRegisterInfo &MRI,
                                            MCContext &Ctx) {
  return new VanillaMCCodeEmitter(MRI, true);
}

void VanillaMCCodeEmitter::
encodeInstruction(const MCInst &MI, raw_ostream &OS,
                  SmallVectorImpl<MCFixup> &Fixups,
                  const MCSubtargetInfo &STI) const
{
  support::endian::Writer<support::little> LE(OS);
  
  // Get instruction encoding and emit it
  uint16_t Value = getBinaryCodeForInstr(MI, Fixups, STI);
  if (IsLittleEndian)
    LE.write<uint16_t>(Value);
}

unsigned VanillaMCCodeEmitter::getMachineOpValue(const MCInst &MI,
                                             const MCOperand &MO,
                                             SmallVectorImpl<MCFixup> &Fixups,
                                             const MCSubtargetInfo &STI) const {
  if (MO.isReg())
    return MRI.getEncodingValue(MO.getReg());
  if (MO.isImm())
    return static_cast<unsigned>(MO.getImm());
  
  assert(MO.isExpr());
  
  const MCExpr *Expr = MO.getExpr();
  
  assert(Expr->getKind() == MCExpr::SymbolRef);
  
  if (MI.getOpcode()==Vanilla::BL
       || MI.getOpcode()==Vanilla::BEQZ
       || MI.getOpcode()==Vanilla::BGTZ
       || MI.getOpcode()==Vanilla::BLTZ
       || MI.getOpcode()==Vanilla::BNEQZ){
    // bb label 11 bit.
    Fixups.push_back(MCFixup::create(0, Expr, FK_PCRel_1));
  }
//  else if(MI.getOpcode()==Vanilla::BEQZ
//          || MI.getOpcode()==Vanilla::BGTZ
//          || MI.getOpcode()==Vanilla::BLTZ
//          || MI.getOpcode()==Vanilla::BNEQZ){
    // bb label 6 bit
//    Fixups.push_back(MCFixup::create(0,Expr,FK_PCRel_2));
//  }
  else if(MI.getOpcode()==Vanilla::JAL){
    // call target (11 bit)
    Fixups.push_back(MCFixup::create(0, Expr, FK_PCRel_4));
  }
  else{
    Fixups.push_back(MCFixup::create(0, Expr, FK_SecRel_1));
  }
  
  return 0;
}

#include "VanillaGenMCCodeEmitter.inc"

