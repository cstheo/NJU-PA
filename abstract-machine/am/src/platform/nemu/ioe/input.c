#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t *i8042_data_port = (uint32_t *)KBD_ADDR;
  kbd->keydown = (i8042_data_port[0] & KEYDOWN_MASK) != 0;
  kbd->keycode = i8042_data_port[0] & ~KEYDOWN_MASK;
}
