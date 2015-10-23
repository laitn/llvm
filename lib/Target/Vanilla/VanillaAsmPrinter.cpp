//===-- VanillaAsmPrinter.cpp - Vanilla LLVM Assembly Printer -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to Vanilla assembly language.
//
//===----------------------------------------------------------------------===//

#include "VanillaAsmPrinter.h"

#include "InstPrinter/VanillaInstPrinter.h"
#include "MCTargetDesc/VanillaBaseInfo.h"
#include "Vanilla.h"
#include "VanillaInstrInfo.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/Twine.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Mangler.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

#define DEBUG_TYPE "asm-printer"

void VanillaAsmPrinter::printOperand(const MachineInstr *MI, int OpNum,
                                 raw_ostream &O, const char *Modifier) {
  const MachineOperand &MO = MI->getOperand(OpNum);
  
  switch (MO.getType()) {
    case MachineOperand::MO_Register:
      O << VanillaInstPrinter::getRegisterName(MO.getReg());
      break;
      
    case MachineOperand::MO_Immediate:
      O << MO.getImm();
      break;
      
    case MachineOperand::MO_MachineBasicBlock:
      O << *MO.getMBB()->getSymbol();
      break;
      
    case MachineOperand::MO_GlobalAddress:
      O << *getSymbol(MO.getGlobal());
      break;
      
    default:
      llvm_unreachable("<unknown operand type>");
  }
}
void VanillaAsmPrinter::EmitInstruction(const MachineInstr *MI) {
  VanillaMCInstLower MCInstLowering(OutContext, *this);
  
  MCInst TmpInst;
  MCInstLowering.Lower(MI, TmpInst);
  EmitToStreamer(*OutStreamer, TmpInst);
}
// Force static initialization.
extern "C" void LLVMInitializeVanillaAsmPrinter() {
    RegisterAsmPrinter<VanillaAsmPrinter> X(TheVanillaTarget);
}
