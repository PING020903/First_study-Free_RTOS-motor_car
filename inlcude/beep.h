#ifndef _BEEP_H_
#define _BEEP_H_
#include "sys.h"

void beep_init(void);

#define BEEP(x)		PFout(8)=(x)


#endif
