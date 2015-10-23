//===-- VanillaSubtarget.h - Define Subtarget for the Vanilla -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Vanilla specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_VANILLA_VANILLASUBTARGET_H
#define LLVM_LIB_TARGET_VANILLA_VANILLASUBTARGET_H

#include "VanillaFrameLowering.h"
#include "VanillaISelLowering.h"
#include "VanillaInstrInfo.h"
#include "llvm/Target/TargetSelectionDAGInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetSubtargetInfo.h"

#define GET_SUBTARGETINFO_HEADER
#include "VanillaGenSubtargetInfo.inc"

namespace llvm {
class StringRef;

class VanillaSubtarget : public VanillaGenSubtargetInfo {
  virtual void anchor();
  TargetSelectionDAGInfo TSInfo;
  VanillaInstrInfo InstrInfo;
  VanillaFrameLowering FrameLowering;
  VanillaTargetLowering TLInfo;

public:
  // This constructor initializes the data members to match that
  // of the specified triple.
  VanillaSubtarget(const Triple &TT, const std::string &CPU, const std::string &FS,
               const TargetMachine &TM);

  // ParseSubtargetFeatures - Parses features string setting specified
  // subtarget options.  Definition of function is auto generated by tblgen.
  void ParseSubtargetFeatures(StringRef CPU, StringRef FS);

  const VanillaInstrInfo *getInstrInfo() const override { return &InstrInfo; }
  const VanillaFrameLowering *getFrameLowering() const override {
    return &FrameLowering;
  }
  const VanillaTargetLowering *getTargetLowering() const override {
    return &TLInfo;
  }
  const TargetSelectionDAGInfo *getSelectionDAGInfo() const override {
    return &TSInfo;
  }
  const TargetRegisterInfo *getRegisterInfo() const override {
    return &InstrInfo.getRegisterInfo();
  }
};
} // End llvm namespace

#endif

