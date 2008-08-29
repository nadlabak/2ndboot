#include "types.h"
#include "atag_priv.h"
#include "images.h"
#include "stdio.h"
#include "atag.h"

void *atag_build() {
  struct buffer_tag *buf;
  struct tag *tag = (struct tag*)ATAG_BASE_ADDR;
	
  tag->hdr.tag = ATAG_CORE;
  tag->hdr.size = tag_size (tag_core);
  tag->u.core.flags = 0;
  tag->u.core.pagesize = 0x00001000;
  tag->u.core.rootdev = 0x0000;
  tag = tag_next(tag);

  if ((buf = image_find(IMG_CMDLINE))) {
    char *atag_cmdline = tag->u.cmdline.cmdline;

    tag->hdr.tag = ATAG_CMDLINE;
    tag->hdr.size = (sizeof(struct tag_header) + buf->size + 1 + 3) >> 2;
    memcpy(atag_cmdline, buf->data, buf->size);
    if (atag_cmdline[buf->size-1] == '\xa') {
      atag_cmdline[buf->size-1] = '\0';
    } else {
      atag_cmdline[buf->size] = '\0';
    }
    printf("cmdline: %s\n", atag_cmdline);
    tag = tag_next(tag);
  }

  tag->hdr.tag = ATAG_MEM;
  tag->hdr.size = tag_size (tag_mem32);
  tag->u.mem.start = 0x90000000;
  tag->u.mem.size = 58*1024*1024;
  tag = tag_next(tag);

  tag->hdr.tag = ATAG_MBM_VERSION;
  tag->hdr.size = tag_size(tag_mbm_version);
  tag->u.mbm_version.mbm_version = 0x1234;
  tag = tag_next(tag);

  if ((buf = image_find(IMG_MOTFLATTREE))) {
    tag->hdr.tag = ATAG_FLAT_DEV_TREE_ADDRESS;
    tag->hdr.size = tag_size (tag_flat_dev_tree_address);
    tag->u.flat_dev_tree_address.flat_dev_tree_address = (u32)buf->data;
    tag->u.flat_dev_tree_address.flat_dev_tree_size = (u32)buf->size;
    tag = tag_next(tag);
  }

  if ((buf = image_find(IMG_INITRAMFS))) {
    tag->hdr.tag = ATAG_INITRD2;
    tag->hdr.size = tag_size(tag_initrd);
    tag->u.initrd.start = (u32)buf->data;
    tag->u.initrd.size = (u32)buf->size;
    tag = tag_next(tag);
  }

  if ((buf = image_find(IMG_USBFW))) {
    tag->hdr.tag = ATAG_USB_FIRMWARE_ADDRESS;
    tag->hdr.size = tag_size (tag_usb_firmware_address);
    tag->u.usb_firmware_address.usb_firmware_address = (u32)buf->data;
    tag = tag_next(tag);

    tag->hdr.tag = ATAG_USB_FIRMWARE_SIZE;
    tag->hdr.size = tag_size (tag_usb_firmware_size);
    tag->u.usb_firmware_size.usb_firmware_size = (u32)buf->size;
    tag = tag_next(tag);
  }

  tag->hdr.tag = ATAG_NONE;
  tag->hdr.size = 0;
  return (void*)ATAG_BASE_ADDR;
}


