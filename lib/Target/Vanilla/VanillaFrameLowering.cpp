//===-- VanillaFrameLowering.cpp - Vanilla Frame Information ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Vanilla implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "VanillaFrameLowering.h"
#include "VanillaInstrInfo.h"
#include "VanillaSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

bool VanillaFrameLowering::hasFP(const MachineFunction &MF) const { return false; }

void VanillaFrameLowering::emitPrologue(MachineFunction &MF,
                                    MachineBasicBlock &MBB) const {
  MachineBasicBlock::iterator MBBI = MBB.begin();
  DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();
  MachineFrameInfo *MFI = MF.getFrameInfo();
  const VanillaInstrInfo &TII = *static_cast<const VanillaInstrInfo *>(MF.getSubtarget().getInstrInfo());
  
  // Get the number of bytes to allocate from the FrameInfo
  int NumBytes = (int) MFI->getStackSize();
  if(NumBytes!=0){
    if(NumBytes<=0x3ff){
      BuildMI(MBB, MBBI, dl, TII.get(Vanilla::MOVI), Vanilla::R1).addImm(NumBytes);
      BuildMI(MBB, MBBI, dl, TII.get(Vanilla::SUBU), Vanilla::R3).addReg(Vanilla::R3).addReg(Vanilla::R1);
    }
    else if(NumBytes<=0xfffff){
      BuildMI(MBB, MBBI, dl, TII.get(Vanilla::MOVI), Vanilla::R1).addImm(NumBytes>>10);
      BuildMI(MBB, MBBI, dl, TII.get(Vanilla::APPI), Vanilla::R1).addReg(Vanilla::R1).addImm(NumBytes& 0x3ff);
      BuildMI(MBB, MBBI, dl, TII.get(Vanilla::SUBU), Vanilla::R3).addReg(Vanilla::R3).addReg(Vanilla::R1);
    }
    else{
      llvm_unreachable("large stack size is not handled.");
    }
  }
}

void VanillaFrameLowering::emitEpilogue(MachineFunction &MF,
                                    MachineBasicBlock &MBB) const {
  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  const VanillaInstrInfo &TII = *static_cast<const VanillaInstrInfo *>(MF.getSubtarget().getInstrInfo());
  DebugLoc dl = MBBI->getDebugLoc();
  MachineFrameInfo *MFI = MF.getFrameInfo();
  
  // Get the number of bytes to allocate from the FrameInfo
  int NumBytes = (int) MFI->getStackSize();
  if(NumBytes!=0){
    if(NumBytes<=0x3ff){
      BuildMI(MBB, MBBI, dl, TII.get(Vanilla::MOVI), Vanilla::R1).addImm(NumBytes);
      BuildMI(MBB, MBBI, dl, TII.get(Vanilla::ADDU), Vanilla::R3).addReg(Vanilla::R3).addReg(Vanilla::R1);
    }
    else if(NumBytes<=0xfffff){
      BuildMI(MBB, MBBI, dl, TII.get(Vanilla::MOVI), Vanilla::R1).addImm(NumBytes>>10);
      BuildMI(MBB, MBBI, dl, TII.get(Vanilla::APPI), Vanilla::R1).addReg(Vanilla::R1).addImm(NumBytes& 0x3ff);
      BuildMI(MBB, MBBI, dl, TII.get(Vanilla::ADDU), Vanilla::R3).addReg(Vanilla::R3).addReg(Vanilla::R1);
    }
    else{
      llvm_unreachable("large stack size is not handled.");
    }
  }
}

void VanillaFrameLowering::determineCalleeSaves(MachineFunction &MF,
                                            BitVector &SavedRegs,
                                            RegScavenger *RS) const {
  TargetFrameLowering::determineCalleeSaves(MF, SavedRegs, RS);
}
