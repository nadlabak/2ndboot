#ifndef __HW_DSP_H__
#define __HW_DSP_H__

#include "types.h"

int dsp_mem_read_word(addr_t src, uint32_t *dest);
int dsp_mem_write_word(addr_t dest, uint32_t word);
int dsp_mem_read(addr_t src, uint32_t *dest, unsigned int words);
int dsp_mem_write(addr_t dest, uint32_t *src, unsigned int words);

/* int dsp_start_loader(addr_t paddr); */
/* int dsp_start_image(addr_t paddr); */

#endif // __HW_DSP_H__
