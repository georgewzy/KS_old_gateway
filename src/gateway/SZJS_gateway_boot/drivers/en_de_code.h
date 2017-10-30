#ifndef __EN_DE_CODE__
#define __EN_DE_CODE__


#include <stdint.h>

#define KEY_DEFAULT 87181


// #include "stdio.h"
// #include "string.h"


extern void encode(uint8_t *src, uint8_t *des, uint32_t size);
extern void decode(uint8_t *src, uint8_t *des, uint32_t size);


#endif
