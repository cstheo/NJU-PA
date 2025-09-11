#ifndef __TRACER_H__
#define __TRACER_H__

#include <elf.h>
#include <common.h>
#include <cpu/decode.h>

#define FUNC_TRACER_SIZE 4096
#define INST_TRACER_SIZE 32
#define MEM_TRACER_SIZE  32

static const char ELF_MAGIC_NUMBER[] = {0x7f, 'E', 'L', 'F'};

typedef concat3(Elf, WORD_WIDTH, _Ehdr) Elf_Ehdr;
typedef concat3(Elf, WORD_WIDTH, _Shdr) Elf_Shdr;
typedef concat3(Elf, WORD_WIDTH, _Sym)  Elf_Sym;
typedef concat3(Elf, WORD_WIDTH, _Ehdr) Elf_Ehdr;
typedef concat3(Elf, WORD_WIDTH, _Off)  Elf_Off;

#define ELF_ST_TYPE concat3(ELF, WORD_WIDTH, _ST_TYPE)

enum FUNC_OP {
  CALL,
  RET,
};

enum MEM_OP {
  MEM_READ,
  MEM_WRITE,
};

typedef struct FuncEntry {
  char *name;
  paddr_t addr;
  paddr_t end; 
  struct FuncEntry *next;
} FuncEntry;

typedef struct {
  bool inited;
  int start, end;
  FuncEntry *funcs;
  struct Trace {
    paddr_t addr;
    int type;
    word_t pc;
  } traces[FUNC_TRACER_SIZE];
} FuncTracer;

typedef struct {
  int start, end;
  char inst[INST_TRACER_SIZE][128];
} InstTracer;

typedef struct {
  int start, end;
  struct {
    paddr_t addr;
    int len;
    int type;
    word_t pc;
  } mem[MEM_TRACER_SIZE];
} MemTracer;

void ftrace_init(char *, Elf_Sym *, word_t);
void ftrace_insert(paddr_t, int);
void ftrace_display();

void itrace_insert(Decode *);
void itrace_display();

void mtrace_insert(paddr_t, int, int);
void mtrace_display();

#endif
