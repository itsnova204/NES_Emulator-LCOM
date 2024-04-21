#pragma once
//this file is used to automaticly include the correct libraries for testing outside of the Minix environment

#ifdef TEST

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BIT(n) (0x01 << n)

#else

#include <lcom/lcf.h>

#endif