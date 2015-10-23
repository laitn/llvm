//===-- VanillaAsmBackend.cpp - Vanilla Assembler Backend -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/VanillaMCTargetDesc.h"
#include "MCTargetDesc/VanillaFixupKinds.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCDirectives.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
class VanillaAsmBackend : public MCAsmBackend {
public:
  Triple::OSType OSType;
  bool IsLittleEndian;

  VanillaAsmBackend(bool IsLittleEndian)
    : MCAsmBackend(), IsLittleEndian(IsLittleEndian) {}
  ~VanillaAsmBackend() override {}
  /*
  const MCFixupKindInfo &getFixupKindInfo(MCFixupKind Kind) const override {
    
    const static MCFixupKindInfo InfosBE[Vanilla::NumTargetFixupKinds] = {
      // name                    offset bits  flags
      { "fixup_vanilla_pc_11",        0,  11,  0 },
      { "fixup_vanilla_pc_re6",       0,  6,  MCFixupKindInfo::FKF_IsPCRel },
      { "fixup_vanilla_pc_re11",      0,  6,  MCFixupKindInfo::FKF_IsPCRel }
    };
    
    const static MCFixupKindInfo InfosLE[Vanilla::NumTargetFixupKinds] = {
      // name                    offset bits  flags
      { "fixup_vanilla_pc_11",        0,  11,  0 },
      { "fixup_vanilla_pc_re6",       0,  6,  MCFixupKindInfo::FKF_IsPCRel },
      { "fixup_vanilla_pc_re11",      0,  6,  MCFixupKindInfo::FKF_IsPCRel }
    };
    
    if (Kind < FirstTargetFixupKind)
      return MCAsmBackend::getFixupKindInfo(Kind);
      
    assert(unsigned(Kind - FirstTargetFixupKind) < getNumFixupKinds() &&
             "Invalid kind!");
    if (IsLittleEndian)
      return InfosLE[Kind - FirstTargetFixupKind];
    
    return InfosBE[Kind - FirstTargetFixupKind];
  }
   */

  void applyFixup(const MCFixup &Fixup, char *Data, unsigned DataSize,
                  uint64_t Value, bool IsPCRel) const override;

  MCObjectWriter *createObjectWriter(raw_pwrite_stream &OS) const override;

  // No instruction requires relaxation
  bool fixupNeedsRelaxation(const MCFixup &Fixup, uint64_t Value,
                            const MCRelaxableFragment *DF,
                            const MCAsmLayout &Layout) const override {
    return false;
  }

  unsigned getNumFixupKinds() const override { return Vanilla::NumTargetFixupKinds; }

  bool mayNeedRelaxation(const MCInst &Inst) const override { return false; }

  void relaxInstruction(const MCInst &Inst, MCInst &Res) const override {}

  bool writeNopData(uint64_t Count, MCObjectWriter *OW) const override;
};

bool VanillaAsmBackend::writeNopData(uint64_t Count, MCObjectWriter *OW) const {
  return true;
}

void VanillaAsmBackend::applyFixup(const MCFixup &Fixup, char *Data,
                               unsigned DataSize, uint64_t Value,
                               bool IsPCRel) const {
  if (Fixup.getKind() == FK_PCRel_1){
    //11 bit
    Data[Fixup.getOffset()]=(Value>>1)& 0xff;
    Data[Fixup.getOffset()+1]=((Value>>9)& 0x7) | Data[Fixup.getOffset()+1];
  }
  else if (Fixup.getKind() == FK_PCRel_2){
    //6 bit
    assert(Value<=0x3f && "6 bit field is overflow.");
    Data[Fixup.getOffset()]=((Value>>1)& 0x3f) | Data[Fixup.getOffset()];
  }
  else if (Fixup.getKind() == FK_PCRel_4){
    //11 bit
    Data[Fixup.getOffset()]=(Value-1)& 0xff;
    Data[Fixup.getOffset()+1]=(((Value-1)>>8)& 0x7) | Data[Fixup.getOffset()+1];
  }
  else if(Fixup.getKind()==FK_SecRel_1){
    //11 bit
    assert(Value == 0);
  }
  else{
    llvm_unreachable("unknown fixup kinds.");
  }
}

MCObjectWriter *VanillaAsmBackend::createObjectWriter(raw_pwrite_stream &OS) const {
  return createVanillaELFObjectWriter(OS, MCELFObjectTargetWriter::getOSABI(OSType), IsLittleEndian);
}
}

MCAsmBackend *llvm::createVanillaAsmBackend(const Target &T,
                                        const MCRegisterInfo &MRI,
                                        const Triple &TT, StringRef CPU) {
  return new VanillaAsmBackend(/*IsLittleEndian=*/true);
}
