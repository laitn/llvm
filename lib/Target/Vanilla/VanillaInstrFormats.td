//===-- VanillaInstrFormats.td - Vanilla Instruction Formats -------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

class InstVanilla<dag outs, dag ins, string asmstr, list<dag> pattern>
  : Instruction {
  field bits<16> Inst;

  let Namespace = "Vanilla";
  let Size = 2;

  bits<5> Opcode;
  let Inst{15-11} = Opcode;               // Top five bits are the 'opcode' field

  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString   = asmstr;
  let Pattern = pattern;

  let DecoderNamespace = "Vanilla";
  field bits<16> SoftFail = 0;
}

// instructions with implicit extra ops.
//class InstVanillaEx<dag outs, dag ins, string asmstr, list<dag> pattern>
//: Instruction {
//  field bits<22> Inst;
  
//  let Namespace = "Vanilla";
//  let Size = 3;
  
//  bits<5> Opcode;
//  let Inst{21-17} = Opcode;               // Top five bits are the 'opcode' field
  
//  dag OutOperandList = outs;
//  dag InOperandList = ins;
//  let AsmString   = asmstr;
//  let Pattern = pattern;
  
//  let DecoderNamespace = "Vanilla";
//  field bits<22> SoftFail = 0;
//}

// Pseudo instructions
class Pseudo<dag outs, dag ins, string asmstr, list<dag> pattern>
  : InstVanilla<outs, ins, asmstr, pattern> {
    let isPseudo = 1;
    let isCodeGenOnly = 1;
    let Inst{15-0}=0;
}

