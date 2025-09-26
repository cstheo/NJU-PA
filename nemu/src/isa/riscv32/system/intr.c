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

#include <isa.h>
#include "../local-include/reg.h"

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
#ifdef CONFIG_ETRACE
  mcause_t *mcause = (mcause_t *)&cpu.mcause;
  Log("%s " FMT_WORD "; mepc=" FMT_WORD "; mtvec=" FMT_WORD "; mstatus=" FMT_WORD,
    mcause->intr ? "INTR" : "EXCP",
    mcause->code,
    epc,
    cpu.mtvec,
    cpu.mstatus);
#endif
  mstatus_t *mstatus = (mstatus_t *)&cpu.mstatus;
  mstatus->mpie = mstatus->mie;
  mstatus->mie = 0;
  mstatus->mpp = cpu.priv;

  cpu.priv = MMODE;
  cpu.mcause = NO;
  cpu.mepc = epc;
  return cpu.mtvec;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
