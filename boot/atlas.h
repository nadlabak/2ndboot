#ifndef __HW_ATLAS_H__
#define __HW_ATLAS_H__

#include "types.h"

int atlas_init();
int atlas_reg_io(int write, unsigned char reg, uint32_t *p);
int atlas_reg_read(unsigned char reg, uint32_t *v);
int atlas_reg_modify(unsigned char reg, uint32_t mask, uint32_t v);
int atlas_reg_write(unsigned char reg, uint32_t v);
int atlas_reg_partial(unsigned char reg, uint32_t mask, uint32_t data);

#endif // __HW_ATLAS_H__
