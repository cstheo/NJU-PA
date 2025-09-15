#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t *vgactl_port = (uint32_t *)(uintptr_t)VGACTL_ADDR;
  int width = vgactl_port[0] >> 16;
  int height = vgactl_port[0] & 0xffff;
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = width, .height = height,
    .vmemsz = width * height * sizeof(uint32_t)
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  int w = io_read(AM_GPU_CONFIG).width;

  uint32_t (*fb)[w] = (uint32_t (*)[w])(uintptr_t)FB_ADDR;
  uint32_t (*px)[ctl->w] = (uint32_t (*)[ctl->w])ctl->pixels;

  for (int i = 0; i < ctl->h; i++) {
    for (int j = 0; j < ctl->w; j++) {
      fb[ctl->y + i][ctl->x + j] = px[i][j];
    }
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
