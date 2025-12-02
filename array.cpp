#include <assert.h>
#include <stdlib.h>

#include "array.h"

#include "common.h"
#include "string_functions.h"


#define ARRAY_CTOR(name)                                                                                            \
                                                                                                                    \
Tree_status ArrayCtor ## name(Array_with_ ## name* array_with_ ## name, size_t start_capacity) {                    \
    array_with_ ## name->capacity = start_capacity;                                                                 \
    array_with_ ## name->size = 0;                                                                                  \
                                                                                                                    \
    array_with_ ## name->data = (name ## _type_t*)calloc(array_with_ ## name->capacity, sizeof(name ## _type_t));   \
                                                                                                                    \
    if (array_with_ ## name->data == NULL) {                                                                        \
        TREE_CHECK_AND_RETURN_ERRORS(MEMORY_ERROR);                                                                 \
    }                                                                                                               \
                                                                                                                    \
    return SUCCESS;                                                                                                 \
}

ARRAY_CTOR(trees);
ARRAY_CTOR(variables);

#undef ARRAY_CTOR


#define ARRAY_RESIZE(name)                                                                                                                          \
                                                                                                                                                    \
Tree_status ArrayResize ## name(Array_with_ ## name* array_with_ ## name, size_t old_capacity) {                                                    \
    name ## _type_t* temp_data = (name ## _type_t*)my_recalloc(array_with_ ## name->data, array_with_ ## name->capacity * sizeof(name ## _type_t),  \
                                                                                          old_capacity * sizeof(name ## _type_t));                  \
                                                                                                                                                    \
    if (temp_data == NULL)                                                                                                                          \
        TREE_CHECK_AND_RETURN_ERRORS(MEMORY_ERROR);                                                                                                 \
                                                                                                                                                    \
    array_with_ ## name->data = temp_data;                                                                                                          \
                                                                                                                                                    \
    return SUCCESS;                                                                                                                                 \
}

ARRAY_RESIZE(trees);
ARRAY_RESIZE(variables);

#undef ARRAY_RESIZE


#define ARRAY_PUSH(name)                                                                                \
                                                                                                        \
Tree_status ArrayPush ## name(Array_with_ ## name* array_with_ ## name, name ## _type_t new_value) {    \
    if (array_with_ ## name->size >= array_with_ ## name->capacity) {                                   \
        size_t old_capacity = array_with_ ## name->capacity;                                            \
        array_with_ ## name->capacity *= REALLOC_COEFF;                                                 \
                                                                                                        \
        TREE_CHECK_AND_RETURN_ERRORS(ArrayResize ## name(array_with_ ## name, old_capacity));           \
    }                                                                                                   \
                                                                                                        \
    array_with_ ## name->data[array_with_ ## name->size] = new_value;                                   \
    array_with_ ## name->size++;                                                                        \
                                                                                                        \
    return SUCCESS;                                                                                     \
}

ARRAY_PUSH(trees);
ARRAY_PUSH(variables);

#undef ARRAY_PUSH


void ArrayDtorVariables(Array_with_variables* array_with_variables) {
    for (size_t i = 0; i < array_with_variables->size; ++i) {
        free(array_with_variables->data[i]->name);
        free(array_with_variables->data[i]);
    }
}

void ArrayDtorTrees(Array_with_trees* array_with_trees) {
    for (size_t i = 0; i < array_with_trees->size; ++i) {
        free(array_with_trees->data[i]->tree);
        free(array_with_trees->data[i]);
    }
}