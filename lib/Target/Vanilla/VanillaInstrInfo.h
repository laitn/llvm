//===-- VanillaInstrInfo.h - Vanilla Instruction Information ------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Vanilla implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_VANILLA_VANILLAINSTRINFO_H
#define LLVM_LIB_TARGET_VANILLA_VANILLAINSTRINFO_H

#include "VanillaRegisterInfo.h"
#include "llvm/Target/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "VanillaGenInstrInfo.inc"

namespace llvm {

class VanillaInstrInfo : public VanillaGenInstrInfo {
  const VanillaRegisterInfo RI;

public:
  VanillaInstrInfo();

  const VanillaRegisterInfo &getRegisterInfo() const { return RI; }
  void storeRegToStackSlot(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator MBBI, unsigned SrcReg,
                           bool isKill, int FrameIndex,
                           const TargetRegisterClass *RC,
                           const TargetRegisterInfo *TRI) const override;
  
  void loadRegFromStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MBBI, unsigned DestReg,
                            int FrameIndex, const TargetRegisterClass *RC,
                            const TargetRegisterInfo *TRI) const override;
  void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                   DebugLoc DL, unsigned DestReg, unsigned SrcReg,
                   bool KillSrc) const override;
  bool AnalyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB,
                     MachineBasicBlock *&FBB,
                     SmallVectorImpl<MachineOperand> &Cond,
                     bool AllowModify) const override;
  
  unsigned RemoveBranch(MachineBasicBlock &MBB) const override;
  unsigned InsertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
                        MachineBasicBlock *FBB, ArrayRef<MachineOperand> Cond,
                        DebugLoc DL) const override;
  bool expandPostRAPseudo (MachineBasicBlock::iterator MI) const override;
  void expandRET(MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const;
  void expandCALL(MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const;
};
}

#endif

