#include "common.h"
#include "tracer.h"

static InstTracer instTracer;
static MemTracer memTracer;

void itrace_insert(Decode *s) {
    strcpy(instTracer.inst[instTracer.end], s->logbuf);
    instTracer.end = (instTracer.end + 1) % INST_TRACER_SIZE;
    if (instTracer.end == instTracer.start) 
      instTracer.start = (instTracer.start + 1) % INST_TRACER_SIZE;
}

void itrace_display() {
  int i = instTracer.start;
  while (i != instTracer.end) {
    printf("%s\n", instTracer.inst[i]);
    i = (i + 1) % INST_TRACER_SIZE;
  }
}

void mtrace_insert(paddr_t addr, int len, int type) {
  memTracer.mem[memTracer.end].addr = addr;
  memTracer.mem[memTracer.end].type = type;
  memTracer.mem[memTracer.end].len = len;
  memTracer.mem[memTracer.end].pc = cpu.pc;
  memTracer.end = (memTracer.end + 1) % MEM_TRACER_SIZE;
  if (memTracer.end == memTracer.start) 
    memTracer.start = (memTracer.start + 1) % MEM_TRACER_SIZE;
}

void mtrace_display() {
  int p = memTracer.start;
  while (p != memTracer.end) {
    printf(
      "pc=" FMT_WORD " %s addr=" FMT_PADDR " %d bytes\n",
      memTracer.mem[p].pc,
      memTracer.mem[p].type == MEM_READ ? " read" : "write",
      memTracer.mem[p].addr,
      memTracer.mem[p].len
    );
    p = (p + 1) % MEM_TRACER_SIZE;
  }
}
