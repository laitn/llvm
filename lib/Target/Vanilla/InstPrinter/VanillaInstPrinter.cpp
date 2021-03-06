//===-- VanillaInstPrinter.cpp - Convert Vanilla MCInst to asm syntax -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an Vanilla MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "Vanilla.h"
#include "VanillaInstPrinter.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
using namespace llvm;

#define DEBUG_TYPE "asm-printer"

// Include the auto-generated portion of the assembly writer.
#include "VanillaGenAsmWriter.inc"

void VanillaInstPrinter::printInst(const MCInst *MI, raw_ostream &O,
                               StringRef Annot, const MCSubtargetInfo &STI) {
  printInstruction(MI, O);
  printAnnotation(O, Annot);
}

static void printExpr(const MCExpr *Expr, raw_ostream &O) {
#ifndef NDEBUG
  const MCSymbolRefExpr *SRE;
  
  if (const MCBinaryExpr *BE = dyn_cast<MCBinaryExpr>(Expr))
    SRE = dyn_cast<MCSymbolRefExpr>(BE->getLHS());
  else
    SRE = dyn_cast<MCSymbolRefExpr>(Expr);
  assert(SRE && "Unexpected MCExpr type.");
  
  MCSymbolRefExpr::VariantKind Kind = SRE->getKind();
  
  assert(Kind == MCSymbolRefExpr::VK_None);
#endif
  O << *Expr;
}

void VanillaInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                  raw_ostream &O, const char *Modifier) {
  assert((Modifier == 0 || Modifier[0] == 0) && "No modifiers supported");
  if(OpNo >= MI->getNumOperands()){
    // for disassembler.
    return;
  }
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg()) {
    O << getRegisterName(Op.getReg());
  } else if (Op.isImm()) {
    O << (int32_t)Op.getImm();
  } else{
    assert(Op.isExpr() && "Expected an expression");
    printExpr(Op.getExpr(), O);
  }
}

void VanillaInstPrinter::printMemOperand(const MCInst *MI, unsigned OpNo, raw_ostream &O,
                                     const char *Modifier) {
  const MCOperand &RegOp = MI->getOperand(OpNo);
  // register
  if(RegOp.isReg()){
    //for assembler.
    O << getRegisterName(RegOp.getReg());
  }
  else if(RegOp.isImm()){
    //for disassembler.
    O << getRegisterName(RegOp.getImm()+1);
  }
  else{
    llvm_unreachable("unhandled mem operand type.");
  }
}

