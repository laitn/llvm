//===-- VanillaVerifier.cpp - Vanilla verification pass ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This is a verifier for vanilla asm. It verifies two-address form, etc.
//===----------------------------------------------------------------------===//

#include "Vanilla.h"
#include "VanillaSubtarget.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetRegisterInfo.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "vanilla-verfier"

STATISTIC(transform, "Number of instructions transformed.");

namespace {
  struct VanillaVerifier : public MachineFunctionPass {
    /// Target machine description which we query for reg. names, data
    /// layout, etc.
    ///
    TargetMachine &TM;
    const VanillaSubtarget *Subtarget;

    static char ID;
    VanillaVerifier(TargetMachine &tm) : MachineFunctionPass(ID), TM(tm) {}

    const char *getPassName() const override {
      return "Vanilla verification pass.";
    }

    bool runOnMachineBasicBlock0(MachineBasicBlock &MBB);
    bool runOnMachineBasicBlock1(MachineBasicBlock &MBB);
    bool runOnMachineBasicBlock2(MachineBasicBlock &MBB);
    bool runOnMachineFunction(MachineFunction &F) override {
      bool Changed = false;
      Subtarget = &F.getSubtarget<VanillaSubtarget>();
      // 0th pass -- check two-address + R1 dst.
      for (MachineFunction::iterator FI = F.begin(), FE = F.end();
           FI != FE; ++FI)
        Changed |= runOnMachineBasicBlock0(*FI);
      // 1st pass -- 2-address form.
      for (MachineFunction::iterator FI = F.begin(), FE = F.end();
           FI != FE; ++FI)
        Changed |= runOnMachineBasicBlock1(*FI);
      // 2nd pass -- add program terminator to signal hardware.
      if(F.getName().compare("main")==0){
        for (MachineFunction::iterator FI = F.begin(), FE = F.end();
             FI != FE; ++FI)
          Changed |= runOnMachineBasicBlock2(*FI);
      }
      return Changed;
    }
  };
  char VanillaVerifier::ID = 0;
}// end of anonymous namespace

FunctionPass *llvm::createVanillaVerifierPass(TargetMachine &tm) {
  return new VanillaVerifier(tm);
}

bool VanillaVerifier::runOnMachineBasicBlock1(MachineBasicBlock &MBB) {
  bool Changed = false;
  Subtarget = &MBB.getParent()->getSubtarget<VanillaSubtarget>();
  const TargetInstrInfo *TII = Subtarget->getInstrInfo();
  for (MachineBasicBlock::iterator I = MBB.begin(); I != MBB.end(); ) {
    MachineBasicBlock::iterator MI = I;
    if(MI->getNumOperands()==3){
      if(MI->getOperand(0).isReg()
         && MI->getOperand(1).isReg()
         && MI->getOperand(2).isReg()){
        if(MI->getOperand(0).getReg()==MI->getOperand(1).getReg()){
          // correct and do nothing;
        }
        else{
          // rd = rds op rs
          // --->
          // mov r1, rds
          // r1 =r1, rs
          // mov rd, r1
          I->dump();
          BuildMI(MBB, I, I->getDebugLoc(), TII->get(Vanilla::MOV), Vanilla::R1).addReg(MI->getOperand(1).getReg());
          I++;
          assert(
                 (MI->getOperand(0).getReg()!=Vanilla::R1
                  && MI->getOperand(1).getReg()!=Vanilla::R1
                  && MI->getOperand(2).getReg()!=Vanilla::R1)
                 && "R1 is already used.");
          BuildMI(MBB, I, I->getDebugLoc(), TII->get(Vanilla::MOV), MI->getOperand(0).getReg()).addReg(Vanilla::R1);
          MI->getOperand(0).setReg(Vanilla::R1);
          MI->getOperand(1).setReg(Vanilla::R1);
          transform++;
          Changed=true;
          continue;
        }
      }
      else if(MI->getOperand(0).isReg()
              && MI->getOperand(1).isReg()
              && MI->getOperand(2).isImm()){
        // appi r1,rx, imm
        // --->
        // mov  r1,rx
        // appi r1,r1, imm
        if(MI->getOperand(0).getReg()!=MI->getOperand(1).getReg()){
          BuildMI(MBB, I, I->getDebugLoc(), TII->get(Vanilla::MOV), Vanilla::R1).addReg(MI->getOperand(1).getReg());
          MI->getOperand(1).setReg(Vanilla::R1);
          I++;
          transform++;
          Changed=true;
          continue;
        }
      }
    }
    I++;
  }
  return Changed;
}

bool VanillaVerifier::runOnMachineBasicBlock2(MachineBasicBlock &MBB) {
  const TargetInstrInfo *TII = Subtarget->getInstrInfo();
  for (MachineBasicBlock::iterator I = MBB.begin(); I != MBB.end(); I++) {
    if(I->getOpcode()==Vanilla::JALR && I->getOperand(0).isReg() && I->getOperand(0).getReg()==Vanilla::R2){
      BuildMI(MBB, I, I->getDebugLoc(), TII->get(Vanilla::WAIT), Vanilla::R0).addReg(Vanilla::R0);
    }
  }
  return true;
}

bool VanillaVerifier::runOnMachineBasicBlock0(MachineBasicBlock &MBB) {
  bool Changed = false;
  Subtarget = &MBB.getParent()->getSubtarget<VanillaSubtarget>();
  const TargetInstrInfo *TII = Subtarget->getInstrInfo();
  for (MachineBasicBlock::iterator I = MBB.begin(); I != MBB.end(); ) {
    MachineBasicBlock::iterator MI = I;
    if (MI->getOpcode()==Vanilla::MOVI && MI->getOperand(0).getReg()!=Vanilla::R1){
      // re-allocate register using reserved R1.
      I++;
      BuildMI(MBB, I, I->getDebugLoc(), TII->get(Vanilla::MOV), MI->getOperand(0).getReg()).addReg(Vanilla::R1);
      MI->getOperand(0).setReg(Vanilla::R1);
      transform++;
      Changed=true;
      continue;
    }
    else if ((MI->getOpcode()==Vanilla::BEQZ
             || MI->getOpcode()==Vanilla::BNEQZ
             || MI->getOpcode()==Vanilla::BGTZ
             || MI->getOpcode()==Vanilla::BLTZ)
             && MI->getOperand(0).getReg()!=Vanilla::R1){
      // re-allocate register using reserved R1.
      BuildMI(MBB, I, I->getDebugLoc(), TII->get(Vanilla::MOV), Vanilla::R1).addReg(MI->getOperand(0).getReg());
      MI->getOperand(0).setReg(Vanilla::R1);
      transform++;
      I++;
      Changed=true;
      continue;
    }
    else if(MI->getOpcode()==Vanilla::APPI && MI->getOperand(0).getReg()!=Vanilla::R1){
      // re-allocate register using reserved R1.
      I++;
      BuildMI(MBB, I, I->getDebugLoc(), TII->get(Vanilla::MOV), MI->getOperand(0).getReg()).addReg(Vanilla::R1);
      MI->getOperand(0).setReg(Vanilla::R1);
      transform++;
      Changed=true;
      continue;
    }
    I++;
  }
  return Changed;
}

