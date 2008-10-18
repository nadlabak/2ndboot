#ifndef __IMAGES_H__
#define __IMAGES_H__
#include "types.h"
#include "buffers.h"

struct memory_image {
  void *data;
  size_t size;
};

struct memory_image *image_find(uint8_t tag, struct memory_image *dest);
int image_unpack(uint8_t tag, addr_t dest, size_t maxsize);
void image_complete();
void image_dump_stats();

#endif // __IMAGES_H__
