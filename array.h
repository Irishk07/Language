#ifndef ARRAY_H_
#define ARRAY_H_

#include <stdio.h>
#include <stdint.h>

#include "common.h"


const size_t DEFAULT_START_CAPACITY = 64; // it must be > 0
const int REALLOC_COEFF = 2;


Tree_status ArrayCtor(Array_with_data* array_with_data, size_t sizeof_elements, size_t start_capacity);

Tree_status ArrayResize(Array_with_data* array_with_data, size_t old_capacity);

Tree_status ArrayPush(Array_with_data* array_with_data, void* new_element);

void ArrayGetElement(Array_with_data* array_with_data, void* element, size_t index);

void ArrayChangeElement(Array_with_data* array_with_data, void* new_element, size_t index);


#endif // ARRAY_H_