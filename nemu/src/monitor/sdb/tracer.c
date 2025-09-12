#include <elf.h>
#include "common.h"
#include "tracer.h"

#ifdef CONFIG_FTRACE
static FuncTracer funcTracer;
static FuncEntry *funcEntry;

void ftrace_init(char *str_table, Elf_Sym *sym_table, word_t sym_size) {
  for (int i = 0; i < sym_size / sizeof(Elf_Sym); i++) {
    if (ELF_ST_TYPE(sym_table[i].st_info) == STT_FUNC) {
      if (funcEntry == NULL) {
        funcEntry = funcTracer.funcs = malloc(sizeof(FuncEntry));
      } else {
        funcEntry->next = malloc(sizeof(FuncEntry));
        funcEntry = funcEntry->next;
      }
      funcEntry->addr = sym_table[i].st_value;
      funcEntry->end = funcEntry->addr + sym_table[i].st_size;
      funcEntry->name = malloc(strlen(&str_table[sym_table[i].st_name]) + 1);
      strcpy(funcEntry->name, &str_table[sym_table[i].st_name]);
    }
  }
  funcTracer.inited = true;
}

void ftrace_insert(paddr_t addr, int type) {
  if (!funcTracer.inited) return;
  funcTracer.traces[funcTracer.end].addr = addr;
  funcTracer.traces[funcTracer.end].type = type;
  funcTracer.traces[funcTracer.end].pc = cpu.pc;
  funcTracer.end = (funcTracer.end + 1) % FUNC_TRACER_SIZE;
  if (funcTracer.end == funcTracer.start) 
    funcTracer.start = (funcTracer.start + 1) % FUNC_TRACER_SIZE;
}

void ftrace_display() {
  int indent = 0;
  int p = funcTracer.start;
  while (p != funcTracer.end) {
    char *name = "\0";
    struct Trace trace = funcTracer.traces[p];
    FuncEntry *entry = funcTracer.funcs;
    while (entry) {
      if (trace.addr >= entry->addr && trace.addr < entry->end) {
        name = entry->name;
        break;
      }
      entry = entry->next;
    }
    printf(FMT_WORD":", trace.pc);
    if (trace.type == CALL) {
      for (int i = 0; i < indent; i++) { putchar(' '); putchar(' '); }
      indent++;
      printf("call [%s@"FMT_WORD"]\n", name, trace.addr);
    } else {
      if (indent > 0) indent--;
      for (int i = 0; i < indent; i++) { putchar(' '); putchar(' '); }
      printf("ret  [%s@"FMT_WORD"]\n", name, trace.addr);
    }
    p = (p + 1) % FUNC_TRACER_SIZE;
  }
}
#endif

#ifdef CONFIG_ITRACE
static InstTracer instTracer;

void itrace_insert(Decode *s) {
    strcpy(instTracer.inst[instTracer.end], s->logbuf);
    instTracer.end = (instTracer.end + 1) % INST_TRACER_SIZE;
    if (instTracer.end == instTracer.start) 
      instTracer.start = (instTracer.start + 1) % INST_TRACER_SIZE;
}

void itrace_display() {
  int p = instTracer.start;
  while (p != instTracer.end) {
    printf("%s\n", instTracer.inst[p]);
    p = (p + 1) % INST_TRACER_SIZE;
  }
}
#endif

#ifdef CONFIG_MTRACE
static MemTracer memTracer;

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
      memTracer.mem[p].type == READ ? " read" : "write",
      memTracer.mem[p].addr,
      memTracer.mem[p].len
    );
    p = (p + 1) % MEM_TRACER_SIZE;
  }
}
#endif

#ifdef CONFIG_DTRACE
static DevTracer devTracer;

void dtrace_insert(const char *name, word_t data, int type) {
  devTracer.traces[devTracer.end].name = malloc(strlen(name) + 1);
  strcpy(devTracer.traces[devTracer.end].name, name);
  devTracer.traces[devTracer.end].data = data;
  devTracer.traces[devTracer.end].type = type;
  devTracer.traces[devTracer.end].pc = cpu.pc;
  devTracer.end = (devTracer.end + 1) % DEV_TRACER_SIZE;
  if (devTracer.end == memTracer.start)
    devTracer.start = (devTracer.start + 1) % DEV_TRACER_SIZE;
}

void dtrace_display() {
  int p = devTracer.start;
  while (p != devTracer.end) {
    printf(
      "pc=" FMT_WORD " %s %s %d\n",
      devTracer.traces[p].pc,
      devTracer.traces[p].type == READ ? " read" : "write",
      devTracer.traces[p].name,
      devTracer.traces[p].data
    );
    p = (p + 1) % DEV_TRACER_SIZE;
  }
}
#endif
