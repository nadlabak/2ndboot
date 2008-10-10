#include "types.h"
#include "console.h"
#include "font.h"
#include "stdio.h"
#include "gpt.h"
#include "nfc.h"
#include "error.h"
#include "atag.h"
#include "common.h"
#include "memory.h"
#include "hw_misc.h"
#include "mu.h"
#include "images.h"
#include "crc32.h"

void critical_error(error_t err) {
  if (console_initialized()) {
    printf("Critical error %d\n", (int)err);
  }
  while (1);
}

int jump_to_linux(void *img_base, int arch, void *atag_list) {
  int (*linux_func)(int zero, int arch, void *atag_list);

  printf("branching to %p\n", img_base);
  linux_func = (int (*)(int, int, void*))img_base;
  return linux_func(0, arch, atag_list);
}

int main(void *boot_base) {
  struct memory_image image;
  
  font_init();
  console_init(&font_8x8);
  gpt_init();
  nfc_init();
  image_complete();

  printf("e8 loader rev %s.\n", LDR_VERSION);
  image_dump_stats();

  mu_dsp_reset();
  hw_preboot();

  if (image_find(IMG_LINUX, &image) != NULL) {
    jump_to_linux(image.data, 1024, atag_build());
  } else {
    critical_error(IMG_NOT_PROVIDED);
  }

  return 0;
}
