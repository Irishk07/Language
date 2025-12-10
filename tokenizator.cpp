#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizator.h"

#include "array.h"
#include "common.h"
#include "onegin.h"
#include "tree.h"


#define NODE_NUMBER_CTOR(num)                                         \
    NodeCtor(NUMBER, (type_t){.number = num}, NULL, NULL)
#define NODE_VARIABLE_CTOR(index)                                     \
    NodeCtor(VARIABLE, (type_t){.index_variable = index}, NULL, NULL)
#define NODE_OPERATOR_CTOR(op)                                        \
    NodeCtor(OPERATOR, (type_t){.operators = op}, NULL, NULL)


Tree_status Tokenizator(Language* language, const char* file_name) {
    assert(language);
    assert(file_name);

    ReadOnegin(language, file_name); // TODO: remove text buffer from struct
    char* str = language->begin_buffer;

    Tree_node* tree_node = NULL;

    while (str < language->end_buffer) {
        if ('0' <= *str && *str <= '9') {
            tree_node = ReadNumber(&str);
        }    

        else if (('a' <= *str && *str <= 'z') ||
                 ('A' <= *str && *str <= 'Z') ||
                 (*str == '_'))
            tree_node = ReadVariable(language, &str);
        
        else {
            tree_node = CheckSigns(language, &str);

            if (tree_node == NULL) {
                fprintf(stderr, "%s: cur pos %s\n", __func__, str);
                TREE_CHECK_AND_RETURN_ERRORS(UNKNOWN_OPERATOR);
            }
        }

        SkipSpaces(&str);
        SkipComments(&str);

        ArrayPush(&(language->array_with_tokens), &tree_node);
    }

    free(language->begin_buffer);
    language->begin_buffer = NULL;
    language->end_buffer = NULL;
    language->size_buffer = 0;

    return SUCCESS;
}

Tree_node* ReadNumber(char** str) {
    assert(str);

    int val = 0;

    while ('0' <= **str && **str <= '9') { // TODO: scanf
        val = val * 10 + (**str - '0');
        (*str)++;
    }

    return NODE_NUMBER_CTOR(val);
}

Tree_node* ReadVariable(Language* language, char** str) {
    assert(language);
    assert(str);

    char* first_symbol = *str;

    Tree_node* tree_node = NULL;

    (*str)++; // skip first symbol (a-zA-Z_)
    while (('a' <= **str && **str <= 'z') || ('A' <= **str && **str <= 'Z') || **str == '_' || ('0' <= **str && **str <= '9'))
        (*str)++;

    char* name_variable = strndup(first_symbol, (size_t)(*str - first_symbol));
    unsigned long hash_variable = hash_djb2(name_variable);

    tree_node = CheckKeyWords(name_variable, hash_variable);
    if (tree_node != NULL) {
        free(name_variable); 

        return tree_node;
    }

    for (size_t i = 0; i < language->array_with_variables.size; ++i) {
        if (strcmp(name_variable, NameOfVariableFromIndex(language, i)) == 0) {
            free(name_variable);

            return NODE_VARIABLE_CTOR((int)i);
        }
    }

    About_variable about_variable = {.name = name_variable, .value = DEFAULT_VALUE};

    ArrayPush(&(language->array_with_variables), &about_variable);

    return NODE_VARIABLE_CTOR((int)(language->array_with_variables.size - 1));
}

Tree_node* CheckKeyWords(char* name_variable, unsigned long hash_variable) {
    assert(name_variable);

    for (size_t i = 0; i < sizeof(key_words) / sizeof(key_words[0]); ++i) {
        if (hash_variable == key_words[i].hash){
            if (strcmp(name_variable, key_words[i].name) == 0) {
                return NODE_OPERATOR_CTOR(key_words[i].type);
            }    
        }    
    }

    return NULL;
}

Tree_node* CheckSigns(Language* language, char** str) {
    assert(language);
    assert(str);
    
    for (size_t i = 0; i < sizeof(signs) / sizeof(signs[0]); ++i) {
        if (strncmp(*str, signs[i].name, strlen(signs[i].name)) == 0){
            (*str)++;

            return NODE_OPERATOR_CTOR(signs[i].type);
        }    
    }

    return NULL;
}