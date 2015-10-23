//===-- VanillaInstrInfo.cpp - Vanilla Instruction Information ----------*- C++ -*-===//
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

#include "Vanilla.h"
#include "VanillaInstrInfo.h"
#include "VanillaSubtarget.h"
#include "VanillaTargetMachine.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"

#define GET_INSTRINFO_CTOR_DTOR
#include "VanillaGenInstrInfo.inc"

using namespace llvm;

VanillaInstrInfo::VanillaInstrInfo()
    : VanillaGenInstrInfo(Vanilla::ADJCALLSTACKDOWN, Vanilla::ADJCALLSTACKUP) {}

void VanillaInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                                       MachineBasicBlock::iterator I,
                                       unsigned SrcReg, bool IsKill, int FI,
                                       const TargetRegisterClass *RC,
                                       const TargetRegisterInfo *TRI) const {
  DebugLoc DL;
  if (I != MBB.end())
    DL = I->getDebugLoc();
  
  //if (RC == &Vanilla::GPRRegClass)
  BuildMI(MBB, I, DL, get(Vanilla::SW))
    .addReg(SrcReg, getKillRegState(IsKill))
  .addFrameIndex(FI);
    //.addImm(0);
  //else
  //  llvm_unreachable("Can't store this register to stack slot");
}

void VanillaInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                                        MachineBasicBlock::iterator I,
                                        unsigned DestReg, int FI,
                                        const TargetRegisterClass *RC,
                                        const TargetRegisterInfo *TRI) const {
  DebugLoc DL;
  if (I != MBB.end())
    DL = I->getDebugLoc();
  
  //if (RC == &Vanilla::GPRRegClass)
  BuildMI(MBB, I, DL, get(Vanilla::LW), DestReg).addFrameIndex(FI);
  //else
  //  llvm_unreachable("Can't load this register from stack slot");
}

void VanillaInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                               MachineBasicBlock::iterator I, DebugLoc DL,
                               unsigned DestReg, unsigned SrcReg,
                               bool KillSrc) const {
  if (Vanilla::GPRRegClass.contains(DestReg, SrcReg))
    BuildMI(MBB, I, DL, get(Vanilla::MOV), DestReg)
    .addReg(SrcReg, getKillRegState(KillSrc));
  else
    llvm_unreachable("Impossible reg-to-reg copy");
}

bool VanillaInstrInfo::AnalyzeBranch(MachineBasicBlock &MBB,
                                 MachineBasicBlock *&TBB,
                                 MachineBasicBlock *&FBB,
                                 SmallVectorImpl<MachineOperand> &Cond,
                                 bool AllowModify) const {
  // Start from the bottom of the block and work up, examining the
  // terminator instructions.
  MachineBasicBlock::iterator I = MBB.end();
  while (I != MBB.begin()) {
    --I;
    if (I->isDebugValue())
      continue;
    
    // Working from the bottom, when we see a non-terminator
    // instruction, we're done.
    if (!isUnpredicatedTerminator(I))
      break;
    
    // A terminator that isn't a branch can't easily be handled
    // by this analysis.
    if (!I->isBranch())
      return true;
    
    // Handle unconditional branches.
    if (I->getOpcode() == Vanilla::BL) {
      if (!AllowModify) {
        TBB = I->getOperand(0).getMBB();
        continue;
      }
      
      // If the block has any instructions after a J, delete them.
      while (std::next(I) != MBB.end())
        std::next(I)->eraseFromParent();
      Cond.clear();
      FBB = 0;
      
      // Delete the J if it's equivalent to a fall-through.
      if (MBB.isLayoutSuccessor(I->getOperand(0).getMBB())) {
        TBB = 0;
        I->eraseFromParent();
        I = MBB.end();
        continue;
      }
      
      // TBB is used to indicate the unconditinal destination.
      TBB = I->getOperand(0).getMBB();
      continue;
    }
    // Cannot handle conditional branches
    return true;
  }
  
  return false;
}

unsigned VanillaInstrInfo::InsertBranch(MachineBasicBlock &MBB,
                                    MachineBasicBlock *TBB,
                                    MachineBasicBlock *FBB,
                                    ArrayRef<MachineOperand> Cond,
                                    DebugLoc DL) const {
  // Shouldn't be a fall through.
  assert(TBB && "InsertBranch must not be told to insert a fallthrough");
  
  if (Cond.empty()) {
    // Unconditional branch
    assert(!FBB && "Unconditional branch with multiple successors!");
    BuildMI(&MBB, DL, get(Vanilla::BL)).addMBB(TBB);
    return 1;
  }
  
  llvm_unreachable("Unexpected conditional branch");
}

unsigned VanillaInstrInfo::RemoveBranch(MachineBasicBlock &MBB) const {
  MachineBasicBlock::iterator I = MBB.end();
  unsigned Count = 0;
  
  while (I != MBB.begin()) {
    --I;
    if (I->isDebugValue())
      continue;
    if (I->getOpcode() != Vanilla::BL)
      break;
    // Remove the branch.
    I->eraseFromParent();
    I = MBB.end();
    ++Count;
  }
  
  return Count;
}

bool VanillaInstrInfo::expandPostRAPseudo (MachineBasicBlock::iterator MI) const{
  MachineBasicBlock &MBB = *MI->getParent();
  
  switch(MI->getDesc().getOpcode()) {
    default:
      return false;
    case Vanilla::RET:
      expandRET(MBB, MI);
      break;
    case Vanilla::CALL:
      expandCALL(MBB, MI);
  }
  MBB.erase(MI);
  return true;
}

void VanillaInstrInfo::expandRET(MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const{
  BuildMI(MBB, I, I->getDebugLoc(), get(Vanilla::JALR)).addReg(Vanilla::R2);
}

void VanillaInstrInfo::expandCALL(MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const{
  BuildMI(MBB, I, I->getDebugLoc(), get(Vanilla::JAL)).addGlobalAddress(I->getOperand(0).getGlobal());
}
