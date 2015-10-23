//===-- VanillaSubtarget.cpp - Vanilla Subtarget Information ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Vanilla specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "VanillaSubtarget.h"
#include "Vanilla.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "vanilla-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "VanillaGenSubtargetInfo.inc"

void VanillaSubtarget::anchor() {}

VanillaSubtarget::VanillaSubtarget(const Triple &TT, const std::string &CPU,
                           const std::string &FS, const TargetMachine &TM)
    : VanillaGenSubtargetInfo(TT, CPU, FS), InstrInfo(), FrameLowering(*this),
      TLInfo(TM, *this) {}

