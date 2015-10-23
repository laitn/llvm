//===-- VanillaELFObjectWriter.cpp - Vanilla ELF Writer ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/VanillaMCTargetDesc.h"
#include "MCTargetDesc/VanillaFixupKinds.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace {
class VanillaELFObjectWriter : public MCELFObjectTargetWriter {
public:
  VanillaELFObjectWriter(uint8_t OSABI);

  ~VanillaELFObjectWriter() override;

protected:
  unsigned GetRelocType(const MCValue &Target, const MCFixup &Fixup,
                        bool IsPCRel) const override;
};
}

VanillaELFObjectWriter::VanillaELFObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter(/*Is64Bit*/ false, OSABI, ELF::EM_NONE,
                              /*HasRelocationAddend*/ false) {}

VanillaELFObjectWriter::~VanillaELFObjectWriter() {}

unsigned VanillaELFObjectWriter::GetRelocType(const MCValue &Target,
                                          const MCFixup &Fixup,
                                          bool IsPCRel) const {
  // determine the type of the relocation
  switch ((unsigned)Fixup.getKind()) {
    default:
      llvm_unreachable("invalid fixup kind!");
    case FK_SecRel_1:
      return ELF::R_X86_64_16;
  }
}

MCObjectWriter *llvm::createVanillaELFObjectWriter(raw_pwrite_stream &OS,
                                               uint8_t OSABI, bool IsLittleEndian) {
  MCELFObjectTargetWriter *MOTW = new VanillaELFObjectWriter(OSABI);
  return createELFObjectWriter(MOTW, OS, IsLittleEndian);
}

