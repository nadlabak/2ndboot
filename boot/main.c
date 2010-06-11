#include "types.h"
#include "stdio.h"
#include "error.h"
#include "atag.h"
#include "common.h"
#include "memory.h"
#include "hw_misc.h"
#include "images.h"
#include "dsp.h"
#include "atlas.h"
#include "images.h"

#define ARCH_NUMBER 2196

void critical_error(error_t err) {
  if (console_initialized()) {
    printf("Critical error %d\n", (int)err);
  }
  while (1);
}

void __attribute__((__naked__)) enter_kernel(int zero, int arch, int *atags, int kern_addr) {
    __asm__ volatile (
        "bx r3\n"
    );
}


int main() {
  struct memory_image image;
  
  image_complete();

  printf("milestone loader rev %s.\n", LDR_VERSION);
  image_dump_stats();

  //dsp_reboot();
  //hw_preboot();

  if (image_find(IMG_LINUX, &image) != NULL) {
    enter_kernel(0, ARCH_NUMBER, atag_build(), KERNEL_DEST);
    //jump_to_linux(image.data, 1024, atag_build());
  } else {
    critical_error(IMG_NOT_PROVIDED);
  }

  return 0;
}
