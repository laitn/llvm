//===- DESELECT.cpp - Code for operator strength reduction ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements removal of 'select' instruction.
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Approx.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "deselect"

STATISTIC(OpCounter, "Counts number of select removed.");

namespace {
  struct DESELECT : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    DESELECT() : FunctionPass(ID) {
      initializeDESELECTPass(*PassRegistry::getPassRegistry());
    }

    bool runOnFunction(Function &F) override ;
  };
}

char DESELECT::ID = 0;
INITIALIZE_PASS(DESELECT, "deselect", "Remove select instructions.", false, false)

Pass *llvm::createDESELECTPass() {
  return new DESELECT();
}


bool DESELECT::runOnFunction(Function &Fn){
  for (Function::iterator BB = Fn.begin(); BB != Fn.end(); ++BB) {
    for(BasicBlock::iterator I=BB->begin();I!=BB->end();){
      if (strcmp(I->getOpcodeName(),"select")==0){
        llvm_unreachable("DESELECT not implemented yet.");
        I++;
      }
      else{
        I++;
      }
    }
  }
  if(OpCounter!=0){
    errs()<<"Func name:"<<Fn.getName()<<"\t";
    errs()<<"Number of select transformed:"<<OpCounter<<"\n";
  }
  return false;
}
