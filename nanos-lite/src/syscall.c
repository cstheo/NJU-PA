#include <common.h>
#include "syscall.h"

#ifdef CONFIG_STRACE
const char* syscall_names[] = {
    "SYS_exit", "SYS_yield", "SYS_open", "SYS_read",
    "SYS_write", "SYS_kill", "SYS_getpid", "SYS_close",
    "SYS_lseek", "SYS_brk", "SYS_fstat", "SYS_time",
    "SYS_signal", "SYS_execve", "SYS_fork", "SYS_link",
    "SYS_unlink", "SYS_wait", "SYS_times", "SYS_gettimeofday"
};

static void strace_call(uintptr_t *a) {
  Log("Syscall %s args=[%p, %p, %p]", syscall_names[a[0]], a[1], a[2], a[3]);
}

static void strace_ret(uintptr_t *a, uintptr_t ret) {
  Log("syscall %s return=0x%x", syscall_names[a[0]], (int)ret);
}
#endif

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

#ifdef CONFIG_STRACE
  strace_call(a);
#endif

  uintptr_t r = 0;
  switch (a[0]) {
    case SYS_exit: r = sys_exit(a[0]); break; 
    case SYS_yield: r = sys_yield(); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

#ifdef CONFIG_STRACE
  strace_ret(a, r);
#endif

  c->GPRx = r;
}
