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
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <memory/vaddr.h>
#include "sdb.h"
#include "debug.h"
#include "tracer.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_si(char *args) {
  int count = args == NULL ? 1 : atoi(args);
  cpu_exec(count);
  return 0;
}

static int cmd_info(char *args) {
  if (!strcmp(args, "r")) {
    isa_reg_display();
  } else if (!strcmp(args, "w")) {
    show_wp();
  } else {
    panic("[sdb/cmd_info]: Invalid param!");
  }
  return 0;
}

static int cmd_p(char *args) {
  bool success = true;
  int value = expr(args, &success);
  if (success) {
    printf("value: %u\n", value);
  }
  else {
    Error("Print value failed!");
  }
  return 0;
}

static int cmd_x(char *args) {
  Assert(args != NULL, "[sdb/cmd_x]: Cmd_x args shoudn't be null");
  char *arg0 = strtok(args, " ");
  char *arg1 = strtok(NULL, " ");
  int64_t count = arg1 == NULL ? 1 : strtol(arg0, NULL, 0);

  bool success = true;
  int value = expr(arg1 == NULL ? arg0 : arg1, &success);
  if (!success) {
    Error("Print value failed!");
    return 0;
  }
  while (count--) {
    printf(""FMT_PADDR": ", value);
    for (int i = sizeof(word_t) - 1; i >= 0; i--) {
      printf("%02x ", vaddr_read(value + i, 1));
    }
    printf("\n");
    value += sizeof(word_t);
  }
  return 0;
}

static int cmd_w(char *args) {
#ifdef CONFIG_WATCHPOINT
  set_wp(args);
#else 
  Error("[sdb/cmd_w]: No support watchpoint");
#endif
  return 0;
}

static int cmd_d(char *args) {
#ifdef CONFIG_WATCHPOINT
  free_wp(atoi(args));
#else 
  Error("[sdb/cmd_d]: No support watchpoint");
#endif
  return 0;
}

static int cmd_itrace(char *args) {
#ifdef CONFIG_ITRACE
  itrace_display();
#else 
  Error("[sdb/cmd_itrace]: No support instruction trace");
#endif
  return 0;
}

static int cmd_mtrace(char *args) {
#ifdef CONFIG_MTRACE
  mtrace_display();
#else 
  Error("[sdb/cmd_mtrace]: No support memory trace");
#endif
  return 0;
}

static int cmd_ftrace(char *args) {
#ifdef CONFIG_FTRACE
  ftrace_display();
#else 
  Error("[sdb/cmd_ftrace]: No support function trace");
#endif
  return 0;
}

static int cmd_dtrace(char *args) {
#ifdef CONFIG_DTRACE
  dtrace_display();
#else 
  Error("[sdb/cmd_dtrace]: No support device trace");
#endif
  return 0;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help},
  { "c", "Continue the execution of the program", cmd_c},
  { "q", "Exit NEMU", cmd_q},
  {"si", "Execute a command", cmd_si},
  {"info", "Print the info of reg/watchpoint", cmd_info},
  {"p", "Print the value of the expression", cmd_p},
  {"x", "Scan N consecutive bytes", cmd_x},
  {"w", "Set a new watchpoint", cmd_w},
  {"d", "Delete a watchpoint", cmd_d},
  {"itrace", "display the instruction trace", cmd_itrace},
  {"mtrace", "display the memory access trace", cmd_mtrace},
  {"ftrace", "display the function trace", cmd_ftrace},
  {"dtrace", "display the device trace", cmd_dtrace}
};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%6s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
#ifdef CONFIG_TEST_EXPR
  /* Test the expresston eval */
  test_expr();
#endif
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
