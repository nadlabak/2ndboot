#include "types.h"
#include "phone.h"
#include "atag_priv.h"
#include "atag.h"

void *atag_build() {
  struct tag *tag = (struct tag*)ATAG_BASE_ADDR;
	
  tag->hdr.tag = ATAG_CORE;
  tag->hdr.size = tag_size (tag_core);
  tag->u.core.flags = 0;
  tag->u.core.pagesize = 0x00001000;
  tag->u.core.rootdev = 0x0000;
  tag = tag_next(tag);

  tag->hdr.tag = ATAG_CMDLINE;
  tag->hdr.size = (sizeof(struct tag_header) + strlen(CMDLINE) + 1 + 3) >> 2;
  strcpy(tag->u.cmdline.cmdline, CMDLINE);
  tag = tag_next(tag);

  tag->hdr.tag = ATAG_MEM;
  tag->hdr.size = tag_size (tag_mem32);
  tag->u.mem.start = 0x90000000;
  tag->u.mem.size = 58*1024*1024;
  tag = tag_next(tag);

  tag->hdr.tag = ATAG_MBM_VERSION;
  tag->hdr.size = tag_size(tag_mbm_version);
  tag->u.mbm_version.mbm_version = 0x1234;
  tag = tag_next(tag);

  tag->hdr.tag = ATAG_NONE;
  tag->hdr.size = 0;
  return (void*)ATAG_BASE_ADDR;
}


