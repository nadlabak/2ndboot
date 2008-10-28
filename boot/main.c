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
#include "dsp.h"
#include "crc32.h"

void critical_error(error_t err) {
  if (console_initialized()) {
    printf("Critical error %d\n", (int)err);
  }
  while (1);
}

int main() {
  struct memory_image image;
  
  image_complete();

  printf("e8 loader rev %s.\n", LDR_VERSION);
  image_dump_stats();

  hw_preboot();

  if (image_find(IMG_LINUX, &image) != NULL) {
    jump_to_linux(image.data, 1024, atag_build());
  } else {
    critical_error(IMG_NOT_PROVIDED);
  }

  return 0;
}
