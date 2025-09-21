#ifndef ARCH_H__
#define ARCH_H__

#include <stdint.h>

#ifdef __riscv_e
#define NR_REGS 16
#else
#define NR_REGS 32
#endif

struct Context {
  uintptr_t gpr[NR_REGS], mcause, mstatus, mepc;
  void *pdir;
};

#ifdef __riscv_e
#define GPR1 gpr[15] // a5
#else
#define GPR1 gpr[17] // a7
#endif

#define GPR2 gpr[10]
#define GPR3 gpr[11]
#define GPR4 gpr[12]
#define GPRx gpr[10]

enum {
  INTR_S_SOFT = 1,
  INTR_M_SOFT = 3,
  INTR_S_TIMR = 5,
  INTR_M_TIMR = 7,
  INTR_S_EXTN = 9,
  INTR_M_EXTN = 11,
};

enum {
  EXCP_INST_UNALIGNED = 0,
  EXCP_INST_ACCESS = 1,
  EXCP_INST = 2,
  EXCP_BREAK = 3,
  EXCP_READ_UNALIGNED = 4,
  EXCP_READ_ACCESS = 5,
  EXCP_STORE_UNALIGNED = 6,
  EXCP_STORE_ACCESS = 7,
  EXCP_U_ENV_CALL = 8,
  EXCP_S_ENV_CALL = 9,
  EXCP_M_ENV_CALL = 11,
  EXCP_INST_PAGE = 12,
  EXCP_READ_PAGE = 13,
  EXCP_STORE_PAGE = 14,
};

enum {
  PTE_V = 0x01,
  PTE_R = 0x02,
  PTE_W = 0x04,
  PTE_X = 0x08,
  PTE_U = 0x10,
  PTE_G = 0x20,
  PTE_A = 0x40,
  PTE_D = 0x80,
};

enum {
  PRIV_MODE_U = 0,
  PRIV_MODE_S = 1,
  PRIV_MODE_M = 3,
};

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

#endif
