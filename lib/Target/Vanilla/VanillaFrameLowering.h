//===-- VanillaFrameLowering.h - Define frame lowering for Vanilla -----*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class implements Vanilla-specific bits of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_VANILLA_VANILLAFRAMELOWERING_H
#define LLVM_LIB_TARGET_VANILLA_VANILLAFRAMELOWERING_H

#include "llvm/Target/TargetFrameLowering.h"

namespace llvm {
class VanillaSubtarget;

class VanillaFrameLowering : public TargetFrameLowering {
public:
  explicit VanillaFrameLowering(const VanillaSubtarget &sti)
      : TargetFrameLowering(TargetFrameLowering::StackGrowsUp, 4, 0) {}

  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  bool hasFP(const MachineFunction &MF) const override;
  void determineCalleeSaves(MachineFunction &MF, BitVector &SavedRegs,
                            RegScavenger *RS) const override;

  void eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator MI) const override {
    MBB.erase(MI);
  }
};
}
#endif

