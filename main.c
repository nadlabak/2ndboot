#include "types.h"
#include "console.h"
#include "font.h"
#include "stdio.h"
#include "gpt.h"
#include "error.h"
// #include "atag.h"

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

int main(void *img_base, uint32_t img_size, uint32_t img_checksum, void *boot_base) {
  int i = 0;

  font_init();
  console_init(&font_8x8);
  gpt_init();

  printf("Welcome. Nothing to see here yet.\n");
  printf("Come back later\n");
  while (1) {
    printf("%d ", i++);
    gpt_wait(1000);
  }
  /*  if (check_image(img_base, img_size, img_checksum) != 0) {
    panic(IMG_INCORRECT_CHECKSUM);
  }
  atag_build(atag_base_addr());
  jump_to_linux(img_base);
  return 0;*/
}
