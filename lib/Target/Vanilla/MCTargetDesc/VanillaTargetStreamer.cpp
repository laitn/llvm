//===-- VanillaTargetStreamer.cpp - Vanilla Target Streamer Methods -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides Vanilla specific target streamer methods.
//
//===----------------------------------------------------------------------===//

#include "VanillaTargetStreamer.h"
#include "InstPrinter/VanillaInstPrinter.h"
#include "llvm/Support/FormattedStream.h"

using namespace llvm;

// pin vtable to this file
VanillaTargetStreamer::VanillaTargetStreamer(MCStreamer &S) : MCTargetStreamer(S) {}

void VanillaTargetStreamer::anchor() {}

VanillaTargetAsmStreamer::VanillaTargetAsmStreamer(MCStreamer &S,
                                               formatted_raw_ostream &OS)
: VanillaTargetStreamer(S), OS(OS) {}

VanillaTargetELFStreamer::VanillaTargetELFStreamer(MCStreamer &S)
: VanillaTargetStreamer(S) {}

MCELFStreamer &VanillaTargetELFStreamer::getStreamer() {
  return static_cast<MCELFStreamer &>(Streamer);
}

