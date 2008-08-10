#include "types.h"
#include "console.h"
#include "font.h"
#include "stdio.h"
#include "gpt.h"
#include "error.h"
#include "atag.h"
#include "common.h"
#include "memory.h"
#include "mxc91231.h"
#include "images.h"
#include "crc32.h"

static struct buffer_tag *gBuffers;

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

struct buffer_tag *image_find(uint32_t tag) {
  struct buffer_tag *buf;
  for (buf = gBuffers; buf->tag != 0; ++buf) {
    if (buf->tag == tag) {
      if (buf->size == 0 || buf->data == NULL) {
        return NULL;
      }
      return buf;
    }
  }
  return NULL;
}

int main(void *boot_base, struct buffer_tag *tag_list) {
  struct buffer_tag *buf;
  int i = 0;
  int wrong_cs = 0;

  gBuffers = tag_list;
  
  font_init();
  console_init(&font_8x8);
  gpt_init();

  printf("Welcome. Nothing to see here yet.\n");
  printf("tag      addr     size     checksum\n");
  for (; tag_list->tag != 0; ++tag_list) {
    printf("%08x %08p %08x %08x ", tag_list->tag, tag_list->data, tag_list->size, tag_list->checksum);
    if (tag_list->size == 0 || tag_list->data == NULL) {
      printf("-");
    } else {
      uint32_t cs = crc32(tag_list->data, tag_list->size);

      if (cs == tag_list->checksum) {
        printf("+");
      } else {
        printf("!", cs);
	wrong_cs++;
      }
    }
    printf("\n");
  }
  if (wrong_cs > 0) {
    critical_error(IMG_INCORRECT_CHECKSUM);
  }
  modify_register32(CRM_AP_BASE_ADDR+0xc, 0, 1 << 20);
  if ((buf = image_find(IMG_LINUX)) != NULL) {
    jump_to_linux(buf->data, 1024, atag_build());
  } else {
    critical_error(IMG_NOT_PROVIDED);
  }

  return 0;
}
