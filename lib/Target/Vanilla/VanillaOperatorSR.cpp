//===-- X86WinEHState - Insert EH state updates for win32 exceptions ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// All functions using an MSVC EH personality use an explicitly updated state
// number stored in an exception registration stack object. The registration
// object is linked into a thread-local chain of registrations stored at fs:00.
// This pass adds the registration object and EH state updates.
//
//===----------------------------------------------------------------------===//

#include "Vanilla.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;

namespace llvm { void initializeVanillaOperatorSRPassPass(PassRegistry &); }

namespace {
class VanillaOperatorSRPass : public FunctionPass {
public:
  static char ID; // Pass identification, replacement for typeid.

  VanillaOperatorSRPass() : FunctionPass(ID) {
    initializeVanillaOperatorSRPassPass(*PassRegistry::getPassRegistry());
  }

  bool runOnFunction(Function &Fn) override;
  void replaceAllUsesWith_stackload(Value *oldv, Value *newv);
};
}

//FunctionPass *llvm::createVanillaOperatorSRPass() { return new VanillaOperatorSRPass(); }

char VanillaOperatorSRPass::ID = 0;

INITIALIZE_PASS(VanillaOperatorSRPass, "vanilla-osr", "Operator strength reduction.", false, false)

void VanillaOperatorSRPass::replaceAllUsesWith_stackload(Value *oldv, Value *addr){
  for(auto user=oldv->user_begin();user!=oldv->user_end();user++){
    //errs()<<"new use";addr->dump();
    IRBuilder<> Builder((Instruction*)*user);
    Value *load=Builder.CreateLoad(addr);
    for(unsigned int i=0;i<user->getNumOperands();i++){
      if(user->getOperand(i)==oldv){
        //errs()<<"GOT IT:";
        //user->getOperand(i)->dump();
        user->setOperand(i,load);
      }
    }
    //errs()<<"user:";user->dump();
  }
  //oldv->replaceAllUsesWith(newv);
}

bool VanillaOperatorSRPass::runOnFunction(Function &Fn){
  int mul_count=0;
  for (Function::iterator BB = Fn.begin(); BB != Fn.end(); ++BB) {
    for(BasicBlock::iterator I=BB->begin();I!=BB->end();){
      if (strcmp(I->getOpcodeName(),"mul")==0){
        Fn.dump();
        // construct blocks;
        std::string mul_init_name="mul.init"+std::to_string(mul_count);
        std::string mul_cond_name="mul.cond"+std::to_string(mul_count);
        std::string mul_body_name="mul.body"+std::to_string(mul_count);
        std::string mul_inc_name="mul.inc"+std::to_string(mul_count);
        std::string next_name="next"+std::to_string(mul_count);
        BasicBlock *mul_init=BB->splitBasicBlock(I, mul_init_name);
        BasicBlock::iterator split_inst=I;split_inst++;
        BasicBlock *next=mul_init->splitBasicBlock(split_inst, next_name);
        
        // insert instructions;
        // I = lhs*rhs;
        // ->
        // res=0;
        // counter=0;
        // do
        //   if (rhs&1==1)
        //     res+=lhs
        //   lhs=lhs<<1
        //   rhs=rhs<<1
        //   counter++
        // while counter<=32
        //->
        //   ...
        //   res_alloc= alloca i32;
        //   counter_alloc= alloca i32;
        //   ...
        // mul.init:
        //   store 0, res_alloc;
        //   store 0, counter_alloc;
        //   br mul.cond
        // mul.cond:
        //   counter = load counter_alloc;
        //   while_cond = icmp slt counter, 32
        //   br while_cond, mul.body, next
        // mul.body:
        //   rhs0 = load rhs_alloc
        //   lhs0 = load lhs_alloc
        //   res0 = load res_alloc
        //   rhs_and_1 = and rhs0, 1;
        //   if_cond =icmp icmp_eq, rhs_and_1, 1;
        //   new_res= add res0, lhs0
        //   res1 = select if_cond, new_res, res0
        //   lhs1 = shra lhs0, 1
        //   rhs1 = shl rhs0, 1
        //   store res1, res_alloc
        //   store lhs1, lhs_alloc
        //   store rhs1, rhs_alloc
        //   br mul.inc
        // mul.inc
        //   counter1 = load counter_alloc
        //   counter_t = add counter1, 1
        //   store counter_t, counter_alloc
        //   br mul.cond
        //
        //
        // allocate stack slot in the beginning of the function
        BasicBlock *first_block=Fn.begin();
        IRBuilder<> Builder_first(first_block,first_block->begin());
        Value *res_alloc=Builder_first.CreateAlloca(I->getType());
        Value *counter_alloc=Builder_first.CreateAlloca(Type::getInt32Ty(getGlobalContext()));
        // ****mul.init
        IRBuilder<> Builder0(mul_init, mul_init->begin());
        Value *lhs=I->getOperand(0);
        Value *rhs=I->getOperand(1);
        Builder0.CreateStore(ConstantInt::get(I->getType(),0),res_alloc);
        Builder0.CreateStore(ConstantInt::get(Type::getInt32Ty(getGlobalContext()),0),counter_alloc);
        // ****mul.cond
        split_inst=mul_init->end();split_inst--;split_inst--;
        BasicBlock *mul_cond=mul_init->splitBasicBlock(split_inst,mul_cond_name);
        IRBuilder<> Builder1(mul_cond, mul_cond->begin());
        Value *counter=Builder1.CreateLoad(counter_alloc);
        Value *while_cond=Builder1.CreateICmp(CmpInst::Predicate::ICMP_SLT, counter, ConstantInt::get(counter->getType(),32));
        // ****mul.body
        split_inst=mul_cond->end();split_inst--;split_inst--;
        BasicBlock *mul_body=mul_cond->splitBasicBlock(split_inst,mul_body_name);
        IRBuilder<> Builder2(mul_body, mul_body->begin());
        Instruction *rhs_load=(Instruction*)rhs;
        Instruction *lhs_load=(Instruction*)lhs;
        assert(strcmp(rhs_load->getOpcodeName(),"load")==0 && "Only variables on memory stack is handled here.");
        assert(strcmp(lhs_load->getOpcodeName(),"load")==0 && "Only variables on memory stack is handled here.");
        Value *rhs0=Builder2.CreateLoad(rhs_load->getOperand(0));
        Value *lhs0=Builder2.CreateLoad(lhs_load->getOperand(0));
        Value *res0=Builder2.CreateLoad(res_alloc);
        Value *rhs_and_1=Builder2.CreateAnd(rhs0, ConstantInt::get(res0->getType(),1));
        Value *if_cond=Builder2.CreateICmp(CmpInst::Predicate::ICMP_EQ, rhs_and_1, ConstantInt::get(rhs_and_1->getType(),1));
        Value *new_res=Builder2.CreateAdd(res0,lhs0);
        Value *res1=Builder2.CreateSelect(if_cond, new_res, res0);
        Value *lhs1=Builder2.CreateLShr(lhs0, ConstantInt::get(lhs0->getType(),1));
        Value *rhs1=Builder2.CreateShl(rhs0, ConstantInt::get(rhs0->getType(),1));
        Builder2.CreateStore(res1, res_alloc);
        Builder2.CreateStore(lhs1, lhs_load->getOperand(0));
        Builder2.CreateStore(rhs1, rhs_load->getOperand(0));
        // ***mul.inc
        split_inst=mul_body->end();split_inst--;split_inst--;
        BasicBlock *mul_inc=mul_body->splitBasicBlock(split_inst, mul_inc_name);
        IRBuilder<> Builder3(mul_inc, mul_inc->begin());
        Value *counter1=Builder3.CreateLoad(counter_alloc);
        Value *counter_t=Builder3.CreateAdd(counter1, ConstantInt::get(counter1->getType(),1));
        Builder3.CreateStore(counter_t, counter_alloc);
        
        // fix control flow.
        BasicBlock::iterator br_mul_inc=mul_inc->end();
        br_mul_inc--;
        br_mul_inc->setOperand(0, mul_cond);
        BasicBlock::iterator br_mul_cond=mul_cond->end();
        br_mul_cond--;
        //br_mul_cond->dump();
        br_mul_cond->eraseFromParent();
        IRBuilder<> Builder4(mul_cond);
        Builder4.CreateCondBr(while_cond, mul_body,next);
        
        // remove multiplication
        replaceAllUsesWith_stackload(I, res_alloc);
        
        //BasicBlock::iterator to_remove=I;
        I++;
        BB=mul_inc;
        //to_remove->eraseFromParent();
        
        Fn.dump();
        mul_count++;
      }
      else{
        I++;
      }
    }
  }
  return false;
}
