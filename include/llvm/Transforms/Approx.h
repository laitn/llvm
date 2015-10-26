//===-- Approx.h - Approximation Transformations --------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This header file defines prototypes for accessor functions that expose passes
// in the Approx Transformations library.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_APPROX_H
#define LLVM_TRANSFORMS_APPROX_H

namespace llvm {
  
  class Pass;
  
  //===----------------------------------------------------------------------===//
  //
  // OSR - Operator strength reduction optimization.
  //
  Pass *createOSRPass();
  
} // End llvm namespace

#endif
