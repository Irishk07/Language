#ifndef ARRAY_H_
#define ARRAY_H_

#include <stdio.h>
#include <stdint.h>

#include "common.h"


const size_t DEFAULT_START_CAPACITY = 32; // it must be > 0
const int REALLOC_COEFF = 2;


Tree_status ArrayCtortrees(Array_with_trees* array_with_trees, size_t start_capacity);

Tree_status ArrayCtorvariables(Array_with_variables* array_with_variables, size_t start_capacity);

Tree_status ArrayResizetrees(Array_with_trees* array_with_trees, size_t old_capacity);

Tree_status ArrayResizevariables(Array_with_variables* array_with_variables, size_t old_capacity);

Tree_status ArrayPushtrees(Array_with_trees* array_with_trees, trees_type_t new_value);

Tree_status ArrayPushvariables(Array_with_variables* array_with_variables, variables_type_t new_value);

void ArrayDtorVariables(Array_with_variables* array_with_variables);

void ArrayDtorTrees(Array_with_trees* array_with_trees);


#endif //ARRAY_H_