#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"

#include "common.h"
#include "string_functions.h"


Tree_status ArrayCtor(Array_with_data* array_with_data, size_t sizeof_elements, size_t start_capacity) {
    array_with_data->capacity = start_capacity;
    array_with_data->size     = 0;
    array_with_data->elem_size = sizeof_elements;

    array_with_data->data = calloc(start_capacity, sizeof_elements);
    if (array_with_data->data == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(MEMORY_ERROR);

    return SUCCESS;
}

Tree_status ArrayResize(Array_with_data* array_with_data, size_t old_capacity) {
    void* temp_data = my_recalloc(array_with_data->data, array_with_data->size * array_with_data->elem_size, old_capacity * array_with_data->elem_size);

    if (temp_data == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(MEMORY_ERROR);

    array_with_data->data = temp_data;

    return SUCCESS;
}

Tree_status ArrayPush(Array_with_data* array_with_data, void* new_element) {
    assert(new_element);

    if (array_with_data->size >= array_with_data->capacity) {
        size_t old_capacity = array_with_data->capacity;
        array_with_data->capacity *= REALLOC_COEFF;

        TREE_CHECK_AND_RETURN_ERRORS(ArrayResize(array_with_data, old_capacity));
    }

    memcpy((char*)array_with_data->data + (array_with_data->size * array_with_data->elem_size), new_element, array_with_data->elem_size);
    array_with_data->size++;

    return SUCCESS;
}

void ArrayGetElement(Array_with_data* array_with_data, void* element, size_t index) {
    memcpy(element, (char*)array_with_data->data + (index * array_with_data->elem_size), array_with_data->elem_size);
}

void ArrayChangeElement(Array_with_data* array_with_data, void* new_element, size_t index) {
    memcpy((char*)array_with_data->data + (index * array_with_data->elem_size), new_element, array_with_data->elem_size);
}