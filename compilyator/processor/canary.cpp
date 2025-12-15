#include <assert.h>
#include <stdio.h>

#include "canary.h"

#include "stack.h"


void SettingCanariesToBegin(type_t* ptr) {
    assert(ptr);
    
    *ptr = (type_t)CANARY;
}

void SettingCanariesToEnd(type_t* ptr, size_t capacity) {
    assert(ptr);
    assert(capacity > 0);
    assert(capacity < MAX_CAPACITY);

    *(ptr + capacity + 1) = (type_t)CANARY;
}