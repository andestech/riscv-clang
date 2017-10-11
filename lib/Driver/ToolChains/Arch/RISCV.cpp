//===--- RISCV.cpp - Tools Implementations ----------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "RISCV.h"
#include "ToolChains/CommonArgs.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/DriverDiagnostic.h"
#include "clang/Driver/Options.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Option/ArgList.h"

using namespace clang::driver;
using namespace clang::driver::tools;
using namespace clang;
using namespace llvm::opt;

const char *riscv::getRISCVTargetCPU(const ArgList &Args,
                                     const llvm::Triple &Triple) {
  StringRef MArch;

  if (const Arg *A = Args.getLastArg(clang::driver::options::OPT_mcpu_EQ)) {
    MArch = A->getValue();
  } else {
    // Otherwise, use the Arch from the triple.
    MArch = Triple.getArchName();
  }

  if (MArch.startswith("riscv32emac"))
    return "rv32emac";
  if (MArch.startswith("riscv32ema"))
    return "rv32ema";
  if (MArch.startswith("riscv32imafd"))
    return "rv32imafd";
  if (MArch.startswith("riscv32imac"))
    return "rv32imac";
  if (MArch.startswith("riscv32ima"))
    return "rv32ima";
  if (MArch.startswith("riscv64imafd"))
    return "rv64imafd";
  if (MArch.startswith("riscv64imac"))
    return "rv64imac";
  if (MArch.startswith("riscv64ima"))
    return "rv64ima";

  bool Is64Bit = Triple.getArch() == llvm::Triple::riscv64;

  if (Arg *A = Args.getLastArg(options::OPT_march_EQ)) {
    StringRef MArch = A->getValue();
    Is64Bit = MArch.startswith("rv64");
  }

  if (Is64Bit)
    return "generic-rv64";

  return "generic-rv32";
}

void riscv::getRISCVTargetFeatures(const Driver &D, const ArgList &Args,
                                   const llvm::Triple &Triple,
                                   std::vector<StringRef> &Features) {
  StringRef MArch, Ext;

  if (const Arg *A = Args.getLastArg(options::OPT_march_EQ)) {
    MArch = A->getValue();
    Ext = MArch.substr(4);
    if (MArch.startswith("rv32")) {
      Features.push_back("+rv32");
    } else if (MArch.startswith("rv64")) {
      Features.push_back("+rv64");
    } else {
      D.Diag(diag::err_drv_invalid_arch_name) << MArch;
    }
  } else {
    // Otherwise, use the Arch from the triple.
    MArch = Triple.getArchName();
    Ext = MArch.substr(7);
    if (MArch.startswith("riscv32")) {
      Features.push_back("+rv32");
    } else if (MArch.startswith("riscv64")) {
      Features.push_back("+rv64");
    } else {
      D.Diag(diag::err_drv_invalid_arch_name) << MArch;
    }
  }

  bool hasF = false, hasD = false, hasE = false;

  for (auto ext : Ext) {
    switch (ext) {
    case 'i':
      break;
    case 'm':
      Features.push_back("+m");
      break;
    case 'a':
      Features.push_back("+a");
      break;
    case 'e':
      Features.push_back("+e");
      hasE = true;
      break;
    case 'f':
      Features.push_back("+f");
      hasF = true;
      break;
    case 'd':
      Features.push_back("+d");
      hasD = true;
      break;
    case 'q':
      D.Diag(diag::err_drv_invalid_arch_name) << MArch;
      break;
    case 'c':
      Features.push_back("+c");
      break;
    case 'g':
      Features.push_back("+m");
      Features.push_back("+a");
      Features.push_back("+f");
      Features.push_back("+d");
      hasF = true;
      hasD = true;
      break;
    default:
      D.Diag(diag::err_drv_invalid_arch_name) << MArch;
    }
  }

  // Dependency check
  if (hasD && !hasF)
    D.Diag(diag::err_drv_invalid_arch_name) << MArch;

  if (hasE && (hasF || hasD))
    D.Diag(diag::err_drv_invalid_arch_name) << MArch;
}

StringRef riscv::getLLVMArchForRISCV(llvm::StringRef CPU, llvm::StringRef Arch,
                                     const llvm::Triple &Triple)
{
  if (CPU.empty() && Arch.empty())
    return Triple.getArchName();
  // Arch order: -march > -mcpu > triple
  StringRef Ext;
  bool Is64Bit = false;

  if (!Arch.empty()) {
    Ext = Arch.substr(4);
    Is64Bit = Arch.startswith("rv64");
  } else if (!CPU.empty()) {
    if (CPU.startswith("rv32")) {
      Ext = CPU.substr(4);
    } else if (CPU.startswith("rv64")) {
      Is64Bit = CPU.startswith("rv64");
      Ext = CPU.substr(4);
    } else {
      if (CPU == "generic-rv64") {
        Is64Bit = true;
      } else if (CPU == "generic-rv32") {
        Is64Bit = false;
      } else {
        // Unknown CPU, just get info from triple
        Ext = Triple.getArchName().substr(7);
        Is64Bit = Triple.isArch64Bit();
      }
    }
  } else {
    Ext = Triple.getArchName().substr(7);
    Is64Bit = Triple.isArch64Bit();
  }

  // Arch has I base ISA at least.
  if (Ext.empty())
    Ext = "i";

  std::string BaseArch = Is64Bit ? "riscv64" : "riscv32";

  return BaseArch + Ext.str();
}
