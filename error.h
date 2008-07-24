#ifndef __ERROR_H__
#define __ERROR_H__
#include "types.h"
#include "console.h"

typedef enum {
  HW_UNINITIALIZED_IPU = 0,
  IMG_CHECKSUM_INVALID,
} error_t;

void critical_error(error_t err);

#endif // __ERROR_H__
