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

#include <common.h>

#ifdef CONFIG_TEST_EXPR
word_t expr(char *e, bool *success);

static void test_expr() {
  FILE *fp = fopen("tools/gen-expr/input.txt", "r");
  if (fp == NULL) {
    printf("Failed to open input.txt\n");
    return;
  }

  char line[65536];
  int pass_count = 0;
  int total_count = 0;

  while (fgets(line, sizeof(line), fp)) {
    if (line[0] == '\n' || line[0] == '\0') continue;
    
    unsigned expected_result;
    char expression[65536];
    int ret = sscanf(line, "%u %[^\n]", &expected_result, expression);
    
    if (ret != 2) continue;
    
    bool success = true;
    unsigned actual_result = expr(expression, &success);
    
    if (success) {
      total_count++;
      if (actual_result == expected_result) {
        pass_count++;
      } else {
        printf("FAIL: %u != %u, expr: %s\n", actual_result, expected_result, expression);
      }
    } else {
      printf("ERROR: Failed to evaluate expression: %s\n", expression);
    }
  }
  
  printf("Expression test: %d/%d passed\n", pass_count, total_count);
  fclose(fp);
}
#endif

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif

#ifdef CONFIG_TEST_EXPR
  test_expr();
#endif
  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
