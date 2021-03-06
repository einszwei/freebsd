//===-- RegisterContextFreeBSD_mips64.cpp ----------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===---------------------------------------------------------------------===//

#include "RegisterContextFreeBSD_mips64.h"
#include "RegisterContextPOSIX_mips64.h"
#include <vector>

using namespace lldb_private;
using namespace lldb;

// http://svnweb.freebsd.org/base/head/sys/mips/include/regnum.h
typedef struct _GPR {
  uint64_t zero;
  uint64_t r1;
  uint64_t r2;
  uint64_t r3;
  uint64_t r4;
  uint64_t r5;
  uint64_t r6;
  uint64_t r7;
  uint64_t r8;
  uint64_t r9;
  uint64_t r10;
  uint64_t r11;
  uint64_t r12;
  uint64_t r13;
  uint64_t r14;
  uint64_t r15;
  uint64_t r16;
  uint64_t r17;
  uint64_t r18;
  uint64_t r19;
  uint64_t r20;
  uint64_t r21;
  uint64_t r22;
  uint64_t r23;
  uint64_t r24;
  uint64_t r25;
  uint64_t r26;
  uint64_t r27;
  uint64_t gp;
  uint64_t sp;
  uint64_t r30;
  uint64_t ra;
  uint64_t sr;
  uint64_t mullo;
  uint64_t mulhi;
  uint64_t badvaddr;
  uint64_t cause;
  uint64_t pc;
  uint64_t ic;
  uint64_t dummy;
} GPR_freebsd_mips;

//---------------------------------------------------------------------------
// Include RegisterInfos_mips64 to declare our g_register_infos_mips64
// structure.
//---------------------------------------------------------------------------
#define DECLARE_REGISTER_INFOS_MIPS64_STRUCT
#include "RegisterInfos_mips64.h"
#undef DECLARE_REGISTER_INFOS_MIPS64_STRUCT

RegisterContextFreeBSD_mips64::RegisterContextFreeBSD_mips64(
    const ArchSpec &target_arch)
    : RegisterInfoInterface(target_arch) {}

size_t RegisterContextFreeBSD_mips64::GetGPRSize() const {
  return sizeof(GPR_freebsd_mips);
}

const RegisterInfo *RegisterContextFreeBSD_mips64::GetRegisterInfo() const {
  assert(m_target_arch.GetCore() == ArchSpec::eCore_mips64);
  return g_register_infos_mips64;
}

uint32_t RegisterContextFreeBSD_mips64::GetRegisterCount() const {
  return static_cast<uint32_t>(sizeof(g_register_infos_mips64) /
                               sizeof(g_register_infos_mips64[0]));
}
