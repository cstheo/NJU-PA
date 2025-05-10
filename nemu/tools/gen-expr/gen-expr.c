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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

static char buf[65536] = {};
static char code_buf[65536 + 128] = {};
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

static uint64_t buf_ptr = 0;

static int choose(int n) {
  return rand() % n;
}

static void gen_num() {
  int num = rand() % 10 + 1;
  int len = sprintf(buf + buf_ptr, "%d", num);
  buf_ptr += len;
}

static void gen_rand_op() {
  const char* ops[] = {"+", "-", "*", "/", "%"};
  int num_ops = sizeof(ops) / sizeof(ops[0]);
  
  int op_index = choose(num_ops);
  const char* op = ops[op_index];
  
  int op_len = strlen(op);
  for (int i = 0; i < op_len; i++) {
    buf[buf_ptr++] = op[i];
  }
  
  if (choose(2)) {
    buf[buf_ptr++] = ' ';
  }
}

static void gen(char s) {
  buf[buf_ptr++] = s;
  
  if ((s == '(' || s == ')') && choose(2)) {
    buf[buf_ptr++] = ' ';
  }
}

static int check_buf_overflow(int needed) {
  return buf_ptr + needed >= sizeof(buf) - 1;
}

static int max_depth = 0;
static int curr_depth = 0;

static void gen_rand_expr() {
  if (curr_depth == 0) {
    buf_ptr = 0;
    buf[0] = '\0';
    max_depth = 10 + rand() % 10;
  }
  
  if (check_buf_overflow(64) || curr_depth >= max_depth) {
    gen_num();
    return;
  }
  
  curr_depth++;
  switch (choose(4)) {
    case 0: gen_num(); break;
    case 1: gen('('); gen_rand_expr(); gen(')'); break;
    default: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
  }
  curr_depth--;
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    curr_depth = 0;
    gen_rand_expr();
    buf[buf_ptr] = '\0';

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc -Werror /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) {
      i--;
      continue;
    }

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    unsigned result;
    ret = fscanf(fp, "%u", &result);
    pclose(fp);

    if (ret == 1) {
      printf("%u %s\n", result, buf);
    }
  }
  return 0;
}
