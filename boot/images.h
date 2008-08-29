#ifndef __IMAGES_H__
#define __IMAGES_H__
#include "types.h"

#define IMG_LINUX		1
#define IMG_INITRAMFS		2
#define IMG_MOTFLATTREE		3
#define IMG_CMDLINE		4
#define IMG_USBFW		5

#pragma pack(push,1)
struct buffer_tag {
  uint32_t tag;
  void *data;
  uint32_t size;
  uint32_t checksum;
};
#pragma pack(pop)

struct buffer_tag *image_find(uint32_t tag);

#endif // __IMAGES_H__
