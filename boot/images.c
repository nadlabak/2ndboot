#include "images.h"
#include "buffers.h"
#include "memory.h"
#include "stdio.h"

uint8_t unpack_buffer(addr_t dest, void *handle);

struct buffer_handle {
  struct abstract_buffer abstract;
  void *rest;
  addr_t dest;
  uint32_t maxsize;
  uint32_t attrs;
  uint32_t reserved[1];
};

struct buffer_handle buffers_list[IMG_LAST_TAG+1] = {
  [IMG_LINUX] = {
    .dest = KERNEL_DEST,
    .maxsize = 3*1024*1024,
  },
  [IMG_INITRAMFS] = {
    .dest = 0x84000000,
    .maxsize = 2*1024*1024,
  },
  [IMG_MOTFLATTREE] = {
    .dest = 0x85000000,
    .maxsize = 1*1024*1024,
  },
  [IMG_CMDLINE] = {
    .dest = 0x85100000,
    .maxsize = 1024,
  },
};

struct memory_image *image_find(uint8_t tag, struct memory_image *dest) {
  if (tag > IMG_LAST_TAG) {
    return NULL;
  }
  if (buffers_list[tag].abstract.state == B_STAT_COMPLETED) {
    dest->data = (void*)buffers_list[tag].dest;
    dest->size = (size_t)buffers_list[tag].abstract.size;
    return dest;
  }
  return NULL;
}

struct memory_image *image_unpack(uint8_t tag, struct memory_image *dest) {
  if (tag > IMG_LAST_TAG) {
    return NULL;
  }
  if (dest->size < buffers_list[tag].abstract.size) {
    return NULL;
  }
  if (buffers_list[tag].abstract.state == B_STAT_CREATED) {
    if (unpack_buffer((addr_t)dest->data, &buffers_list[tag]) == B_STAT_COMPLETED) {
      return dest;
    }
  }
  return NULL;
}

void image_complete() {
  int i;
  struct abstract_buffer *ab;

  for (i = 1; i <= IMG_LAST_TAG; ++i) {
    ab = &buffers_list[i].abstract;

    if (ab->state == B_STAT_CREATED) {
    }
    if ((ab->state == B_STAT_COMPLETED) &&
	(ab->attrs & B_ATTR_VERIFY)) {
      if (ab->checksum != crc32(buffers_list[i].dest, (size_t)ab->size)) {
	ab->state = B_STAT_CRCERROR;
      }
    }
  }
}

void u_to_hex(int x, int digits, char *s){
    int i, digit;
    for (i=0;i<digits;i++) {
      digit=x&0x0f;
      s[digits-1-i]=(digit>9)?(digit-10+'a'):(digit+'0');
      x>>=4;
    }
}

void image_dump_stats() {
  int i;
  struct abstract_buffer *ab;
  char s[]="xxxx yyyyyyyy zzzzzzzz c\n";

  printf("tag  addr     size\n");
  for (i = 1; i <= IMG_LAST_TAG; ++i) {
    int c;

    ab = &buffers_list[i].abstract;

    switch (ab->state) {
    case B_STAT_NONE:
      c = '-'; break;
    case B_STAT_CREATED:
      c = '*'; break;
    case B_STAT_COMPLETED:
      c = '+'; break;
    case B_STAT_CRCERROR:
      c = '!'; break;
    case B_STAT_OVERFLOW:
      c = '^'; break;
    case B_STAT_ERROR:
      c = '#'; break;
    default:
      c = '?'; break;
    }

    u_to_hex(i, 4, s);
    u_to_hex(buffers_list[i].dest, 8, s+5);
    u_to_hex(ab->size, 8, s+14);
    s[23]=c;
    printf("%s", s);

//    printf("%04d %08x %08x %c\n", (uint32_t)i, buffers_list[i].dest, ab->size, (char)c);
  }
}
