#ifndef STRING_FUNCTIONS_H_
#define STRING_FUNCTIONS_H_

#include <stdio.h>

#include "stack.h"


void *my_recalloc(void *ptr, size_t new_size, size_t old_size);

void init_with_poisons(type_t* ptr, size_t size);


#endif //STRING_FUNCTIONS_H_