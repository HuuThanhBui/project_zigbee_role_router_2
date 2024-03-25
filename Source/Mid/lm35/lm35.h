#ifndef _LM35_H_
#define _LM35_H_

#include "stdint.h"

void lm35_init(void);
uint32_t lm35_read(void);

#endif
