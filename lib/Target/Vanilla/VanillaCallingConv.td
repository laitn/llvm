//===-- VanillaCallingConv.td - Calling Conventions Vanilla ----*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the Vanilla architectures.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Vanilla
//===----------------------------------------------------------------------===//

def CC_Vanilla : CallingConv<[
  // Promote i8/i16 args to i32
  CCIfType<[ i8, i16 ], CCPromoteToType<i32>>,
  // All arguments get passed in integer registers if there is space.
  CCIfType<[i32], CCAssignToReg<[R5,R6, R7, R8]>>,

  // Alternatively, they are assigned to the stack in 4-byte aligned units.
  CCAssignToStack<4, 4>
]>;

def RetCC_Vanilla : CallingConv<[
  CCIfType<[i32], CCAssignToReg<[R9,R10]>>
]>;

// Callee-saved registers: stack pointer and link register.
def CSR : CalleeSavedRegs<(add R3, R2)>;

