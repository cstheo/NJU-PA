#include <common.h>
#include "syscall.h"

static int sys_yield() {
  yield();
  return 0;
}

static int sys_exit(int arg0) {
  halt(arg0);
  return 0;
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  uintptr_t r = 0;
  switch (a[0]) {
    case SYS_exit: r = sys_exit(a[0]); break; 
    case SYS_yield: r = sys_yield(); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  c->GPRx = r;
}
