#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_back.h"

#include "../array.h"
#include "../common.h"
#include "onegin.h"
#include "../tree.h"


Tree_status LanguageCtor(Language* language, const char* file_with_tree,
                         const char* html_dump_filename, const char *directory) {
    assert(language);
    assert(file_with_tree);

    language->begin_buffer = NULL;
    language->end_buffer   = NULL;
    language->size_buffer  = 0;

    language->tree_file = file_with_tree;

    language->dump_info.html_dump_filename = html_dump_filename;
    language->dump_info.directory          = directory;

    language->cnt_labels.cnt_if    = 0;
    language->cnt_labels.cnt_while = 0;
    language->cnt_labels.cnt_else  = 0;

    return SUCCESS;
}

bool IsConstantNode(Language* language, Tree_node* node) {
    assert(language);

    if (node == NULL) return false;
    
    switch (node->type) {
        case NUMBER:
            return true;
        case VARIABLE:
            return false;
        case OPERATOR:
            return IsConstantNode(language, node->left_node) && 
                   IsConstantNode(language, node->right_node);
        case WRONG_TYPE:
        default:
            return false;
    }
}

Tree_status ReadPreOrderTreeFile(Language* language) {
    assert(language);

    TREE_CHECK_AND_RETURN_ERRORS(ReadOnegin(language, language->tree_file));

    char* begin_buffer = language->begin_buffer;

    TREE_CHECK_AND_RETURN_ERRORS(ReadPreOrderNode(language, &language->tree.root, &begin_buffer));

    DUMP_CURRENT_SITUATION(language->tree.root);

    free(language->begin_buffer);
    language->begin_buffer = NULL;
    language->end_buffer   = NULL;
    language->size_buffer  = 0;

    return SUCCESS;
}

Tree_status ReadPreOrderNode(Language* language, Tree_node** tree_node, char** current_pos) {    
    assert(language);
    assert(tree_node);
    assert(current_pos);
    assert(*current_pos);

    SkipSpaces(current_pos);

    if (*current_pos > language->end_buffer)
        TREE_CHECK_AND_RETURN_ERRORS(BUFFER_OVERFLOW);

    if (**current_pos == '(') {
        (*current_pos)++;
        SkipSpaces(current_pos);

        if (strncmp(*current_pos, "nil", LEN_NIL) == 0) {
            *current_pos += LEN_NIL;
            *tree_node = NULL;

            SkipSpaces(current_pos);
        }
        else {
            if (**current_pos == '\"') // open_"
                (*current_pos)++;

            int read_bytes = 0;
            sscanf(*current_pos, "%*s%n", &read_bytes);

            if (*(*current_pos + read_bytes - 1) == '\"')
                *(*current_pos + read_bytes - 1) = '\0'; // close_" -> '\0'
            
            char* name = strndup(*current_pos, (size_t)read_bytes);
            type_t value = {.number = 0};

            Type_node type = FindOutType(name, &value);

            if (type != VARIABLE)
                free(name);

            *tree_node = NodeCtor(type, value, NULL, NULL);

            *current_pos += read_bytes;

            SkipSpaces(current_pos);

            TREE_CHECK_AND_RETURN_ERRORS(ReadPreOrderNode(language, &(*tree_node)->left_node, current_pos));

            TREE_CHECK_AND_RETURN_ERRORS(ReadPreOrderNode(language, &(*tree_node)->right_node, current_pos));

            SkipSpaces(current_pos);

            (*current_pos)++; // ++ because skip ')'

            SkipSpaces(current_pos);
        }
    }
    else if (strncmp(*current_pos, "nil", LEN_NIL) == 0) {
        *current_pos += LEN_NIL;

        *tree_node = NULL;

        SkipSpaces(current_pos);
    }
    else {
        fprintf(stderr, "%s\n", *current_pos);
        TREE_CHECK_AND_RETURN_ERRORS(SYNTAX_ERROR);
    }

    return SUCCESS;
}

Tree_status LanguageDtor(Language* language) {
    LanguageNodeDtor(language, language->tree.root);

    return SUCCESS;
}

void LanguageNodeDtor(Language* language, Tree_node* tree_node) {
    if (tree_node == NULL)
        return;

    LanguageNodeDtor(language, tree_node->left_node);
    LanguageNodeDtor(language, tree_node->right_node);

    if (tree_node->type == VARIABLE) {
        free((void*)NameOfVariable(tree_node));
        free(tree_node->value.about_variable);
    }

    tree_node->left_node = NULL;
    tree_node->right_node = NULL;

    free(tree_node);
}

void ArrayDtorVariables(Array_with_data* array_with_data) {
    for (size_t i = 0; i < array_with_data->size; ++i) {
        About_variable* about_variable = NULL;
        ArrayGetElement(array_with_data, &about_variable, i);
        free(about_variable);

    }    
    free(array_with_data->data);
}