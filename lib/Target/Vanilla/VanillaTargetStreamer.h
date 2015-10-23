//===-- VanillaTargetStreamer.h - Vanilla Target Streamer ----------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_VANILLA_VANILLATARGETSTREAMER_H
#define LLVM_LIB_TARGET_VANILLA_VANILLATARGETSTREAMER_H

#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCStreamer.h"

namespace llvm {
class VanillaTargetStreamer : public MCTargetStreamer {
  virtual void anchor();

public:
  VanillaTargetStreamer(MCStreamer &S);
};

// This part is for ascii assembly output
class VanillaTargetAsmStreamer : public VanillaTargetStreamer {
  formatted_raw_ostream &OS;

public:
  VanillaTargetAsmStreamer(MCStreamer &S, formatted_raw_ostream &OS);

};

// This part is for ELF object output
class VanillaTargetELFStreamer : public VanillaTargetStreamer {
public:
  VanillaTargetELFStreamer(MCStreamer &S);
  MCELFStreamer &getStreamer();
};

} // end namespace llvm

#endif

