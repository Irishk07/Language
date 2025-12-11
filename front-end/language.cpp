#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "language.h"

#include "array.h"
#include "../common.h"
#include "onegin.h"
#include "tree.h"


Tree_status LanguageCtor(Language* language, const char* html_dump_filename, const char *directory,
                                             const char* programm_file, const char* tree_file) {
    assert(language);
    assert(html_dump_filename);
    assert(directory);

    language->begin_buffer = NULL;
    language->end_buffer   = NULL;
    language->size_buffer  = 0;

    language->programm_file = programm_file;
    language->tree_file     = tree_file;

    language->dump_info.html_dump_filename = html_dump_filename;
    language->dump_info.directory          = directory;

    language->array_with_variables = {};
    ArrayCtor(&(language->array_with_variables), sizeof(About_variable), DEFAULT_START_CAPACITY);
    language->array_with_tokens = {};
    ArrayCtor(&(language->array_with_tokens), sizeof(Tree_node*), DEFAULT_START_CAPACITY);

    return SUCCESS;
}

Tree_status CreatePreOrderTreeFile(Language* language) {
    assert(language);

    FILE* tree_file = fopen(language->tree_file, "w");
    if (tree_file == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(OPEN_ERROR);

    PrintPreOrderTreeToFile(language, language->tree.root, tree_file);

    if (fclose(tree_file) == EOF)
        TREE_CHECK_AND_RETURN_ERRORS(CLOSE_ERROR);

    return SUCCESS;
}

void PrintPreOrderTreeToFile(Language* language, Tree_node* tree_node, FILE* stream) {
    assert(stream);

    if (tree_node == NULL) {
        fprintf(stream, "nil");
        return;
    }

    if (tree_node->type == NUMBER)
        fprintf(stream, "(%d ", tree_node->value.number);
    
    else if (tree_node->type == VARIABLE)
        fprintf(stream, "(\"%s\" ", NameOfVariable(language, tree_node));

    if (tree_node->type == OPERATOR)
        fprintf(stream, "(%s ", IndetifySign(tree_node));

    PrintPreOrderTreeToFile(language, tree_node->left_node, stream);
    fprintf(stream, " ");

    PrintPreOrderTreeToFile(language, tree_node->right_node, stream);
    fprintf(stream, ")");
}

Tree_status LanguageDtor(Language* language) {
    ArrayDtorVariables(language, &language->array_with_variables);
    free(language->array_with_variables.data);

    free(language->array_with_tokens.data);

    LanguageNodeDtor(language, language->tree.root);

    return SUCCESS;
}

void LanguageNodeDtor(Language* language, Tree_node* tree_node) {
    if (tree_node == NULL)
        return;

    LanguageNodeDtor(language, tree_node->left_node);
    LanguageNodeDtor(language, tree_node->right_node);

    tree_node->left_node = NULL;
    tree_node->right_node = NULL;

    free(tree_node);
}

void ArrayDtorVariables(Language* language, Array_with_data* array_with_data) {
    for (size_t i = 0; i < array_with_data->size; ++i) {
        free(NameOfVariableFromIndex(language, i));
    }
}