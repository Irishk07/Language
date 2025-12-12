#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "middle_end.h"

#include "array.h"
#include "../common.h"
#include "differentiator.h"
#include "onegin.h"
#include "tree.h"


Tree_status LanguageCtor(Language* language, const char* html_dump_filename, const char *directory, const char* tree_file) {
    assert(language);
    assert(html_dump_filename);
    assert(directory);

    language->begin_buffer = NULL;
    language->end_buffer   = NULL;
    language->size_buffer  = 0;

    language->tree_file = tree_file;

    language->dump_info.html_dump_filename = html_dump_filename;
    language->dump_info.directory          = directory;

    language->array_with_variables = {};
    ArrayCtor(&(language->array_with_variables), sizeof(About_variable), DEFAULT_START_CAPACITY);

    return SUCCESS;
}

Tree_status Middle_end(Language* language) {
    assert(language);

    Tree_node* old_root = language->tree.root;

    ReadPreOrderTreeFile(language);

    OptimizationTree(language, &language->tree.root);

    CreatePreOrderTreeFile(language);

    LanguageNodeDtor(language, old_root);

    return SUCCESS;
}

Tree_status CreatePreOrderTreeFile(Language* language) {
    assert(language);

    FILE* file = fopen(language->tree_file, "w");
    if (file == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(OPEN_ERROR);

    PrintPreOrderTreeToFile(language, language->tree.root, file);

    if (fclose(file) == EOF)
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

Tree_status ReadPreOrderTreeFile(Language* language) {
    assert(language);

    TREE_CHECK_AND_RETURN_ERRORS(ReadOnegin(language, language->tree_file));

    char* begin_buffer = language->begin_buffer;

    TREE_CHECK_AND_RETURN_ERRORS(ReadPreOrderNode(language, &language->tree.root, &begin_buffer));

    TREE_CHECK_AND_RETURN_ERRORS(TreeHTMLDump(language, language->tree.root, DUMP_INFO, NOT_ERROR_DUMP));

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

            Type_node type = FindOutType(language, name, &value);

            *tree_node = NodeCtor(type, value, NULL, NULL);

            *current_pos += read_bytes;

            TREE_CHECK_AND_RETURN_ERRORS(ReadPreOrderNode(language, &(*tree_node)->left_node, current_pos));

            TREE_CHECK_AND_RETURN_ERRORS(ReadPreOrderNode(language, &(*tree_node)->right_node, current_pos));

            (*current_pos)++; // ++ because skip ')'

            SkipSpaces(current_pos);
            free(name);
        }
    }

    else if (strncmp(*current_pos, "nil", LEN_NIL) == 0) {
        *current_pos += LEN_NIL;

        *tree_node = NULL;
    }

    else {
        fprintf(stderr, "%s\n", *current_pos);
        TREE_CHECK_AND_RETURN_ERRORS(SYNTAX_ERROR);
    }

    return SUCCESS;
}

Type_node FindOutType(Language* language, const char* name, type_t* value) {
    assert(language);
    assert(name);
    assert(value);

    if (ItIsOperator(name, value) == FIND_YES)
        return OPERATOR;

    if (ItIsNumber(name, value) == FIND_YES)
        return NUMBER;

    if (ItIsVariable(language, name, value) == FIND_YES)
        return VARIABLE;

    return WRONG_TYPE;
}

Status_of_finding ItIsOperator(const char* name, type_t* value) {
    assert(name);
    assert(value);

    unsigned long hash = hash_djb2(name);

    for (size_t i = 0; i < sizeof(key_words) / sizeof(key_words[0]); ++i) {
        if (hash == key_words[i].hash && strcmp(name, key_words[i].name) == 0) {
            value->operators = key_words[i].type;
            return FIND_YES;
        }
    }

    return FIND_NO;
}

Status_of_finding ItIsNumber(const char* name, type_t* value) {
    assert(name);
    assert(value);

    char* endptr = NULL;
    int result = (int)strtol(name, &endptr, 10);

    if (endptr == name || *endptr != '\0')
        return FIND_NO;

    value->number = result;

    return FIND_YES;
}

Status_of_finding ItIsVariable(Language* language, const char* name, type_t* value) {
    assert(language);
    assert(name);
    assert(value);

    if (isdigit(*name))
        return FIND_NO;

    for (size_t i = 0; i < language->array_with_variables.size; ++i) {
        if (strcmp(name, NameOfVariableFromIndex(language, i)) == 0) {
            value->index_variable = (int)i;
            return FIND_YES;
        }
    }

    value->index_variable = (int)language->array_with_variables.size;
    
    About_variable about_variable = {.name = strdup(name), .value = DEFAULT_VALUE};

    ArrayPush(&(language->array_with_variables), &about_variable);
    
    return FIND_YES;
}

Tree_status LanguageDtor(Language* language) {
    ArrayDtorVariables(language, &language->array_with_variables);
    free(language->array_with_variables.data);

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