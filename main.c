#include "types.h"
#include "console.h"
#include "font.h"
#include "stdio.h"
#include "gpt.h"
#include "error.h"
#include "atag.h"

void critical_error(error_t err) {
  if (console_initialized()) {
    printf("Critical error %d\n", (int)err);
  }
  while (1);
}

int jump_to_linux(void *img_base, int arch, void *atag_list) {
  int (*linux_func)(int zero, int arch, void *atag_list);

  linux_func = (int (*)(int, int, void*))img_base;
  return linux_func(0, arch, atag_list);
}

void checksum_update(uint32_t *ctx, void *buf, uint32_t size)
{
  unsigned char *cbuf = (unsigned char*)buf;
  uint32_t i;

  for (i = 0; i < size; ++i) {
    *ctx += cbuf[i];
  }
}

uint32_t checksum(void *buf, uint32_t size) {
  uint32_t ctx = 0;

  checksum_update(&ctx, buf, size);
  return ctx;
}

int main(void *img_base, uint32_t img_size, uint32_t img_checksum, void *boot_base) {
  int i = 0;

  font_init();
  console_init(&font_8x8);
  gpt_init();

  printf("Welcome. Nothing to see here yet.\n");
  printf("%p %08lx %08lx %p\n", img_base, img_size, img_checksum, boot_base);
  if (checksum(img_base, img_size) != img_checksum) {
    critical_error(IMG_INCORRECT_CHECKSUM);
  }
  jump_to_linux(img_base, 1024, atag_build());
  return 0;
}
