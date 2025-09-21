/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#ifndef __RISCV_REG_H__
#define __RISCV_REG_H__

#include <common.h>
#include <isa.h>

static inline int check_reg_idx(int idx) {
  IFDEF(CONFIG_RT_CHECK, assert(idx >= 0 && idx < MUXDEF(CONFIG_RVE, 16, 32)));
  return idx;
}

#define gpr(idx) (cpu.gpr[check_reg_idx(idx)])

typedef struct {
  uintptr_t resv_0 : 1;
  uintptr_t sie : 1;
  uintptr_t resv_1 : 1;
  uintptr_t mie : 1;
  uintptr_t resv_2 : 1;
  uintptr_t spie : 1;
  uintptr_t resv_3 : 1;
  uintptr_t mpie : 1;
  uintptr_t spp : 1;
  uintptr_t resv_4 : 2;
  uintptr_t mpp : 2;
  uintptr_t fs : 2;
  uintptr_t xs : 2;
  uintptr_t mprv : 1;
  uintptr_t sum : 1;
  uintptr_t mxr : 1;
  uintptr_t tvm : 1;
  uintptr_t tw : 1;
  uintptr_t tsr : 1;
#ifdef __ISA_RISCV64__
  uintptr_t resv_5 : 40; // XLEN - 24
#else
  uintptr_t resv_5 : 8; // XLEN - 24
#endif
  uintptr_t sd : 1;
} mstatus_t;

typedef struct {
#ifdef __ISA_RISCV64__
  uintptr_t code : 63;
#else
  uintptr_t code : 31;
#endif
  uintptr_t intr : 1;
} mcause_t;

typedef struct {
#ifdef __ISA_RISCV64__
  uintptr_t ppn : 44;
  uintptr_t asid : 16;
  uintptr_t mode : 4;
#else
  uintptr_t ppn : 22;
  uintptr_t asid : 9;
  uintptr_t mode : 1;
#endif
} satp_t;

static inline const char* reg_name(int idx) {
  extern const char* regs[];
  return regs[check_reg_idx(idx)];
}

static inline word_t *addr2csr(word_t addr) {
  switch (addr) {
    case 0x180: return &cpu.satp;
    case 0x300: return &cpu.mstatus;
    case 0x305: return &cpu.mtvec;
    case 0x341: return &cpu.mepc;
    case 0x342: return &cpu.mcause;
  }
  panic("Unsupport csr addr %x", addr);
}

#define csr(csr_num) (*addr2csr(csr_num))

#endif
