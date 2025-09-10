#ifndef __TRACER_H__
#define __TRACER_H__

#include <cpu/decode.h>

#define INST_TRACER_SIZE 16
#define MEM_TRACER_SIZE 32

typedef struct {
  int start, end;
  char inst[INST_TRACER_SIZE][128];
} InstTracer;

void itrace_insert(Decode *);
void itrace_display();

enum MEM_OP {
  MEM_READ,
  MEM_WRITE,
};

typedef struct {
  int start, end;
  struct {
    paddr_t addr;
    int len;
    int type;
    word_t pc;
  } mem[MEM_TRACER_SIZE];
} MemTracer;

void mtrace_insert(paddr_t, int, int);
void mtrace_display();

#endif
