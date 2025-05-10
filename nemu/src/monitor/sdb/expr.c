/***************************************************************************************
 * Copyright (c) 2014-2024 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan
 *PSL v2. You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
 *KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 *NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include <isa.h>
#include <memory/vaddr.h>
#include <debug.h>
#include <regex.h>

#define TOKEN_MAX 4096
#define TOKEN_BASE 256

enum {
  TK_NOTYPE = TOKEN_BASE,  
  
  TK_NUM,    // num
  TK_HEX,    // 0x
  TK_REG,    // $
  TK_LP,     // (
  TK_RP,     // )
  
  TK_MINUS, // -num
  TK_DEREF, // *(expr)
  
  TK_MUL,   // *
  TK_DIV,   // /
  TK_MOD,   // %
  
  TK_SUB,   // -
  TK_PLUS,  // +
  
  TK_LT,    // <
  TK_GT,    // >
  TK_LE,    // <=
  TK_GE,    // >=
  
  TK_EQ,    // ==
  TK_NE,    // !=
  
  TK_AND,   // &&
  
  TK_OR,    // ||
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {
    {" +", TK_NOTYPE}, // spaces

    {"==", TK_EQ},     // equal
    {"!=", TK_NE},     // not qeual
    {"<=", TK_LE},     // less and equal
    {">=", TK_GE},     // bigger and equal
    {"<", TK_LT},      // less
    {">", TK_GT},      // bigger
    {"&&", TK_AND},    // and
    {"\\|\\|", TK_OR}, // or

    {"\\$[a-zA-Z0-9]+", TK_REG}, // reg

    {"0[xX][0-9a-fA-F]+", TK_HEX}, // hex number
    {"[0-9]+", TK_NUM},            // number

    {"\\+", TK_PLUS}, // plus
    {"-", TK_SUB},    // sub
    {"\\*", TK_MUL},  // mul
    {"/", TK_DIV},    // divide
    {"%", TK_MOD},    // mod
    {"\\(", TK_LP},   // left pare
    {"\\)", TK_RP},   // left pare
};

#define NR_REGEX ARRLEN(rules)
static int priority[NR_REGEX+2] = {
  0, 1, 1, 1, 0, 0, 1, 2,
  2, 2, 2, 3, 3, 4, 4,
  4, 4, 5, 5, 6, 7,
};

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[WORD_SIZE_BITS];
} Token;

static Token tokens[TOKEN_MAX] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 &&
          pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;
        Assert(substr_len < 8 * WORD_SIZE_BITS, "[sdb/make_token]: Too long arg!");

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i,
            rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        switch (rules[i].token_type) {
        case TK_NOTYPE:
          break;
        default:
          Assert(nr_token < TOKEN_MAX, "[sdb/make_token]: Too much tokens!");
          tokens[nr_token].type = rules[i].token_type;
          strncpy(tokens[nr_token].str, substr_start, substr_len);
          tokens[nr_token].str[substr_len] = '\0';
          nr_token++;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  for (int i = 0; i < nr_token; i++) {
    if (tokens[i].type == TK_SUB && (i == 0 || (tokens[i-1].type != TK_NUM && tokens[i-1].type != TK_HEX && 
         tokens[i-1].type != TK_REG && tokens[i-1].type != TK_RP))) {
      tokens[i].type = TK_MINUS;
    }
    if (tokens[i].type == TK_MUL && (i == 0 || (tokens[i-1].type != TK_NUM && tokens[i-1].type != TK_HEX && 
         tokens[i-1].type != TK_REG && tokens[i-1].type != TK_RP))) {
      tokens[i].type = TK_DEREF;
    }
  }
  return true;
}

static bool check_parentheses(int left, int right) {
  bool pass = true;
  if (tokens[left].type != TK_LP || tokens[right].type != TK_RP) {
    pass = false;
  }

  int count = 0;
  for (int i = left; i <= right; i++) {
    if (tokens[i].type == TK_LP) {
      count++;
    } 
    else if (tokens[i].type == TK_RP) {
      count--;
    }
    
    if (count == 0 && i < right) {
      pass = false;
    }
  }

  Assert(count == 0, "[sdb/check_parentheses]: Unmatched parentheses: L:%d/R:%d", left, right);
  return pass;
}

static int get_op(int left, int right) {
  int op = left;
  int count = 0;
  for (int i = left; i <= right; i++) {
    if (tokens[i].type == TK_LP) {
      count++;
      while (count != 0 || tokens[i].type != TK_RP) {
        i++;
        if (tokens[i].type == TK_RP) {
          count--;
        }
        else if (tokens[i].type == TK_LP) {
          count++;
        }
      }
    }
    if (priority[tokens[i].type - TOKEN_BASE] >= priority[tokens[op].type - TOKEN_BASE]) {
      op = i;
    }
  }
  return op;
}

static word_t eval(int left, int right) {
  Log("left:%d, right:%d", left, right);
  Assert(left >= 0 && right < TOKEN_MAX, "[sdb/eval]: Invalid input L:%d/R:%d", left, right);
  Assert(left <= right, "[sdb/eval]: Unexpected behavior: L:%d > R:%d!", left, right);

  if (left == right) {
    Assert(tokens[left].type == TK_NUM || tokens[left].type == TK_HEX || tokens[left].type == TK_REG,
           "[sdb/eval]: Unexpected behavior: Type:%d != TK_NUM/HEX!",tokens[left].type);

    return tokens[left].type == TK_REG ? 
      isa_reg_str2val(tokens[left].str) : 
      strtol(tokens[left].str, NULL, 0);
  } 
  else if (check_parentheses(left, right)) {
    return eval(left + 1, right - 1);
  } 
  else {
    int op = get_op(left, right);
    Log("op: %d", op);
    int val1 = tokens[op].type != TK_MINUS  && tokens[op].type != TK_DEREF ? eval(left, op - 1) : 0;
    int val2 = eval(op + 1, right);

    switch (tokens[op].type) {
      case TK_EQ: return val1 == val2;
      case TK_NE: return val1 != val2;
      case TK_LE: return val1 <= val2;
      case TK_GE: return val1 >= val2;
      case TK_LT: return val1 < val2;
      case TK_GT: return val1 > val2;
      case TK_AND: return val1 && val2; 
      case TK_OR: return val1 || val2;
      case TK_NUM: Assert(0, "[sdb/eval]: Invalid Type: TK_NUM");
      case TK_HEX: Assert(0, "[sdb/eval]: Invalid Type: TK_HEX");
      case TK_MINUS: return -val2;
      case TK_DEREF: return vaddr_read(val2, WORD_SIZE_BYTE);
      case TK_REG: Assert(0, "[sdb/eval]: Invalid Type: TK_REG");
      case TK_PLUS: return val1 + val2;
      case TK_SUB: return val1 - val2;
      case TK_MUL: return val1 * val2;
      case TK_DIV: 
        if (val2 == 0) {
          Assert(0, "[sdb/eval]: Division by zero");
        }
        return val1 / val2;
      case TK_MOD: 
        if (val2 == 0) {
          Assert(0, "[sdb/eval]: Modulo by zero");
        }
        return val1 % val2;
      case TK_LP: Assert(0, "[sdb/eval]: Invalid Type: TK_LP");
      case TK_RP: Assert(0, "[sdb/eval]: Invalid Type: TK_RP");
      default: Assert(0, "[sdb/eval]: Unexpected behavior: Invalid Type: %d",tokens[op].type);
    }
  }
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  *success = true;
  return eval(0, nr_token-1);
}
