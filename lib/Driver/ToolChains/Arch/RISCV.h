//===--- RISCV.h - RISCV-specific Tool Helpers ------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_RISCV_H
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_RISCV_H

#include "clang/Driver/Driver.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Option.h"
#include <string>
#include <vector>

namespace clang {
namespace driver {
namespace tools {
namespace riscv {

void getRISCVTargetFeatures(const Driver &D, const llvm::opt::ArgList &Args,
                            const llvm::Triple &Triple,
                            std::vector<llvm::StringRef> &Features);

const char *getRISCVTargetCPU(const llvm::opt::ArgList &Args,
                              const llvm::Triple &Triple);

StringRef getLLVMArchForRISCV(llvm::StringRef CPU, llvm::StringRef Arch,
                              const llvm::Triple &Triple);

} // end namespace riscv
} // end namespace target
} // end namespace driver
} // end namespace clang

#endif // LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_RISCV_H
