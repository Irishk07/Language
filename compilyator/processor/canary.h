#ifndef CANARY_H_
#define CANARY_H_

#include <stdio.h>

#include "debug.h"
#include "stack.h"

#ifdef CANARIES
const size_t CNT_CANARIES = 2;
#else
const size_t CNT_CANARIES = 0;
#endif //CANARIES


const int CANARY = 0XEDA; // TODO: what if type_t is not an integer type?


void SettingCanariesToBegin(type_t* ptr);

void SettingCanariesToEnd(type_t* ptr, size_t capacity);


#endif //CANARY_H_