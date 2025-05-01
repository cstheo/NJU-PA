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

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  char expression[WORD_SIZE_BITS];
  word_t old_value;
} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

WP* new_wp() {
  if (free_ == NULL) {
    Log(ANSI_FMT("No free watchpoint!", ANSI_FG_RED));
    return NULL;
  }
  WP *wp = free_;
  free_ = free_->next;
  wp->next = head;
  head = wp;
  return wp;
}

bool set_wp(char *args) {
  WP *wp;
  bool success = false;
  word_t value = expr(args, &success);
  if (!success) {
    printf(ANSI_FMT("[sdb/set_wp]: Invalid Input!\n", ANSI_FG_RED));
    return false;
  }
  if (!(wp = new_wp())) {
    printf(ANSI_FMT("[sdb/set_wp]: No free watchpoint!\n", ANSI_FG_RED));
    return false;
  }

  strcpy(wp->expression, args);
  wp->old_value = value;
  return true;
}

bool free_wp(int NO) {
  WP *pre = head;
  WP *current = head;
  while (current->NO != NO) {
    pre = current;
    current = current->next;
  }
  if (current->NO != NO) {
    printf(ANSI_FMT("[sdb/free_wp]: Invalid watchpoint NO!\n", ANSI_FG_RED));
    return false;
  }
  current == head ? (head = current->next) : (pre->next = current->next);

  current->next = free_;
  free_ = current;
  return true;
}

void show_wp() {
  WP *current = head;
  printf("%2s     %18s     %5s\n", "ID", "expression", "value");
  while (current != NULL) {
    printf("%2d     %18s     %5d\n", current->NO, current->expression, current->old_value);
    current = current->next;
  }
}

bool check_wp() {
  WP *current = head;
  bool hit = false;
  while (current != NULL) {
    bool success = false;
    word_t new_value = expr(current->expression, &success);
    if (new_value != current->old_value) {
      printf("Hit watchpoint\n");
      printf("%2d     %18s     %5d\n", current->NO, current->expression, current->old_value);
      hit = true;
    }
    current = current->next;
  }
  return hit;
}
