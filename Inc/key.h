#ifndef _KEY_H_
#define _KEY_H_

enum key_value
{
	ADDR_K  = 1>>0,
	ARM_K 	= 1>>1,
	CAL_K	= 1>>2,
};

void KeyInit(void);
void checkKey(void);

#endif

