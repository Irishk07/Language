#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "differentiator.h"

#include "array.h"
#include "common.h"
#include "tree.h"


Tree_status DifferentiatorCtor(Differentiator* differentiator, const char* html_dump_filename, const char *directory) {
    assert(differentiator);
    assert(html_dump_filename);
    assert(directory);

    differentiator->begin_buffer = NULL;
    differentiator->end_buffer   = NULL;
    differentiator->size_buffer  = 0;

    differentiator->dump_info.html_dump_filename = html_dump_filename;
    differentiator->dump_info.directory          = directory;

    ArrayCtorvariables(&differentiator->array_with_variables, DEFAULT_START_CAPACITY);

    return SUCCESS;
}

Tree_status DifferentiatorDtor(Differentiator* differentiator) {
    // Tree_status status = TreeVerify(differentiator);

    free(differentiator->buffer_with_tree);

    ArrayDtorVariables(&differentiator->array_with_variables);
    free(differentiator->array_with_variables.data);

    DifferentiatorNodeDtor(differentiator, differentiator->tree.root);

    return SUCCESS;
}

void DifferentiatorNodeDtor(Differentiator* differentiator, Tree_node* tree_node) {
    if (tree_node == NULL)
        return;

    DifferentiatorNodeDtor(differentiator, tree_node->left_node);
    DifferentiatorNodeDtor(differentiator, tree_node->right_node);

    tree_node->left_node = NULL;
    tree_node->right_node = NULL;

    free(tree_node);
}