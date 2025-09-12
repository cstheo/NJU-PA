#include <common.h>

#ifdef CONFIG_TEST_EXPR
word_t expr(char *e, bool *success);

void test_expr() {
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
