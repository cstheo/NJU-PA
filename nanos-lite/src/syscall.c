#include <common.h>
#include "syscall.h"
#include "fs.h"

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

static void sys_exit(int status) {
  halt(status);
}

static int sys_read(int fd, void *buf, size_t len) {
  return fs_read(fd, buf, len);
}

static int sys_write(int fd, const void *buf, size_t count) {
    return fs_write(fd, buf, count);
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
    case SYS_exit: sys_exit(a[1]); break; 
    case SYS_yield: r = sys_yield(); break;
    case SYS_read: r = sys_read(a[1], (void *)a[2], a[3]); break;
    case SYS_write: r = sys_write(a[1], (void *)a[2], a[3]); break;
    case SYS_brk: r = 0; break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

#ifdef CONFIG_STRACE
  strace_ret(a, r);
#endif

  c->GPRx = r;
}
