//===-- VanillaRegisterInfo.td - Vanilla Register defs ---------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the Vanilla register file
//===----------------------------------------------------------------------===//

class VanillaReg<bits<16> Enc, string n> : Register<n> {
  let HWEncoding = Enc;
  let Namespace = "Vanilla";
}

let Namespace = "Vanilla" in {
  foreach Index=0-63 in{
    def R#Index: VanillaReg<Index,"r"#Index>, DwarfRegNum<[Index]>;
  }
}

// General purpos register
def GPR : RegisterClass<"Vanilla", [i32], 32, (sequence "R%u", 0, 31)>;

//def GPR_SUB1 : RegisterClass<"Vanilla", [i32], 32, (add R1)>;

// Constant register
def CR : RegisterClass<"Vanilla", [i32], 32, (sequence "R%u", 32, 63)>;

