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

typedef struct
{
  word_t      : 1, sie  : 1,      : 1, mie  : 1,      : 1,
         spie : 1, ube  : 1, mpie : 1, spp  : 1, vs   : 2,
         mpp  : 2, fs   : 2, xs   : 2, mprv : 1, sum  : 1,
         mxr  : 1, tvm  : 1, tw   : 1, tsr  : 1,      : 8, sd   : 1;
} mstatus_t;

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
