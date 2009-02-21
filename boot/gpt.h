#ifndef __HW_GPT_H__
#define __HW_GPT_H__
#include "types.h"

void gpt_init();
void gpt_reset();
void gpt_wait(uint32_t ticks);
void msleep(unsigned int ms);

#endif // __HW_GPT_H__
