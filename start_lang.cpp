#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "array.h"
#include "common.h"
#include "start_lang.h"
#include "tree.h"

// узел список аргументов - он как узел с точкой запятой

// G ::= OP+ '$'
// OP ::= {A | IF | '{'OP+'}' } ';'
// IF ::= "if" '('E')'OP
// A ::= V'='E
// E ::= T{[+ -]T}*
// T ::= POW{[* /]POW}*
// POW ::= P{[^]P}*
// P ::= '('E')' | N | V | F
// N ::= [0-9]+
// V ::= [a-z _][a-z 0-9 _]*
// F ::= [ln, sin, cos, tg, ctg, arcsin, arccos, arctg, arcctg, sh, ch, th, cth]'('E')'

#define DUMP_CURRENT_SITUATION(node)                                                        \
{                                                                                           \
        TreeHTMLDump(differentiator, node, DUMP_INFO, NOT_ERROR_DUMP);                      \
        FILE* html_dump_file = fopen(differentiator->dump_info.html_dump_filename, "a");    \
        fprintf(html_dump_file, "%s: current situation:'%s'", __func__, *str);              \
        fclose(html_dump_file);                                                             \
}

Tree_node* LangGetComandir(Differentiator* differentiator, Tree_status* status, const char* file_name) {
    assert(status);
    assert(file_name);

    SpuskReadOnegin(&differentiator->buffer_with_tree, file_name);
    char* str = differentiator->buffer_with_tree;

    Tree_node* tree_node = NULL;

    do {
        Tree_node* tree_node_2 = LangGetOperators(differentiator, &str, status);

        if (tree_node_2 == NULL)
            break;

        if (tree_node != NULL && tree_node->right_node == NULL)
            tree_node->right_node = tree_node_2;
        else
            tree_node = SpuskNodeCtor(OPERATOR, (type_t){.operators = OPERATOR_PC}, tree_node_2, tree_node);

        TreeHTMLDump(differentiator, tree_node, DUMP_INFO, NOT_ERROR_DUMP);
    }
    while (true);

    if (*str != '$') {
        *status = NOT_END_SYMBOL;
        return NULL;
    }

    return tree_node;
}

Tree_node* LangGetOperators(Differentiator* differentiator, char** str, Tree_status* status) {
    assert(differentiator);
    assert(status);
    assert(str);

    Tree_node* tree_node = LangGetIf(differentiator, str, status);
    if (tree_node != NULL)
        return tree_node;

    DUMP_CURRENT_SITUATION(tree_node);

    tree_node = LangGetAssignment(differentiator, str, status);
    if (tree_node != NULL)
        return tree_node;

    DUMP_CURRENT_SITUATION(tree_node);

    if (**str == '{') {
        (*str)++;

        while(**str != '}') {
            Tree_node* tree_node_2 = LangGetOperators(differentiator, str, status);

            DUMP_CURRENT_SITUATION(tree_node_2);

            if (tree_node == NULL)
                tree_node = tree_node_2;
            else if (tree_node != NULL && tree_node->right_node == NULL)
                tree_node->right_node = tree_node_2;
            else
                tree_node = SpuskNodeCtor(OPERATOR, (type_t){.operators = OPERATOR_PC}, tree_node_2, tree_node);

            // tree_node = SpuskNodeCtor(OPERATOR, (type_t){.operators = OPERATOR_PC}, tree_node_2, tree_node);

            DUMP_CURRENT_SITUATION(tree_node);
        } 
        
        (*str)++;
    }

    DUMP_CURRENT_SITUATION(tree_node);

    return tree_node;
}

Tree_node* LangGetIf(Differentiator* differentiator, char** str, Tree_status* status) {
    assert(differentiator);
    assert(status);
    assert(str);

    if (strncmp(*str, "if", 2) != 0)
        return NULL;

    (*str) += 2;

    if (**str == '(') {
        (*str)++;

        Tree_node* tree_node = LangGetExpression(differentiator, str, status);

        DUMP_CURRENT_SITUATION(tree_node);

        if (**str == ')') {
            (*str)++;

            Tree_node* tree_node_2 = LangGetOperators(differentiator, str, status);

            DUMP_CURRENT_SITUATION(tree_node_2);

            return SpuskNodeCtor(OPERATOR, (type_t){.operators = OPERATOR_IF}, tree_node, tree_node_2);
        }

    }
    return NULL;
}

Tree_node* LangGetAssignment(Differentiator* differentiator, char** str, Tree_status* status) {
    assert(differentiator);
    assert(status);
    assert(str);

    Tree_node* tree_node = LangGetVariable(differentiator, str, status);
    if (tree_node == NULL)
        return NULL;

    DUMP_CURRENT_SITUATION(tree_node);

    if (**str == '=') {
        (*str)++;

        Tree_node* tree_node_2 = LangGetExpression(differentiator, str, status);

        DUMP_CURRENT_SITUATION(tree_node_2);

        fprintf(stderr, "%s: '%s'\n", __func__, *str);

        if (**str != ';')
            return NULL;

        (*str)++;

        tree_node = SpuskNodeCtor(OPERATOR, (type_t){.operators = OPERATOR_EQUAL}, tree_node, tree_node_2);

        DUMP_CURRENT_SITUATION(tree_node);

        return SpuskNodeCtor(OPERATOR, (type_t){.operators = OPERATOR_PC}, tree_node, NULL);
    }

    return NULL;
}

Tree_node* LangGetExpression(Differentiator* differentiator, char** str, Tree_status* status) {
    assert(str);
    assert(*str);
    assert(status);

    Tree_node* new_node = LangGetTerm(differentiator, str, status);

    DUMP_CURRENT_SITUATION(new_node);

    while (**str == '+' || **str == '-') {
        int cur_operator = (**str);
        (*str)++;

        Tree_node* new_node_2 = LangGetTerm(differentiator, str, status);

        DUMP_CURRENT_SITUATION(new_node_2);

        if (cur_operator == '+')
            new_node = SpuskNodeCtor(OPERATOR, (type_t){.operators = OPERATOR_ADD}, new_node, new_node_2);
        else 
            new_node = SpuskNodeCtor(OPERATOR, (type_t){.operators = OPERATOR_SUB}, new_node, new_node_2);
    }

    DUMP_CURRENT_SITUATION(new_node);

    return new_node;
}

Tree_node* LangGetTerm(Differentiator* differentiator, char** str, Tree_status* status) {
    assert(str);
    assert(*str);

    Tree_node* new_node = LangGetPow(differentiator, str, status);

    DUMP_CURRENT_SITUATION(new_node);

    while (**str == '*' || **str == '/') {
        int cur_operator = (**str);
        (*str)++;

        Tree_node* new_node_2 = LangGetPow(differentiator, str, status);
        DUMP_CURRENT_SITUATION(new_node_2);

        if (cur_operator == '*')
            new_node = SpuskNodeCtor(OPERATOR, (type_t){.operators = OPERATOR_MUL}, new_node, new_node_2);
        else 
            new_node = SpuskNodeCtor(OPERATOR, (type_t){.operators = OPERATOR_DIV}, new_node, new_node_2);
    }

    DUMP_CURRENT_SITUATION(new_node);

    return new_node;
}

Tree_node* LangGetPow(Differentiator* differentiator, char** str, Tree_status* status) {
    assert(differentiator);
    assert(status);
    assert(str);

    Tree_node* new_node = LangGetPrimaryExpression(differentiator, str, status);

    DUMP_CURRENT_SITUATION(new_node);

    SkipSpaces(str);
    while (**str == '^') {
        (*str)++;

        Tree_node* new_node_2 = LangGetPrimaryExpression(differentiator, str, status);
        DUMP_CURRENT_SITUATION(new_node_2);
        new_node = SpuskNodeCtor(OPERATOR, (type_t){.operators = OPERATOR_POW}, new_node, new_node_2);
        
        SkipSpaces(str);
    }

    DUMP_CURRENT_SITUATION(new_node);

    return new_node;
}

Tree_node* LangGetPrimaryExpression(Differentiator* differentiator, char** str, Tree_status* status) {
    assert(str);
    assert(*str);
    
    Tree_node* tree_node = NULL;

    if (**str == '(') {
        (*str)++;
        tree_node = LangGetExpression(differentiator, str, status);

        DUMP_CURRENT_SITUATION(tree_node);

        if (**str != ')')
            *status = NOT_END_SKOBKA;

        (*str)++;
    }

    else {
        tree_node = LangGetNumber(differentiator, str, status);
        if (tree_node != NULL)
            return tree_node;

        tree_node = LangGetVariable(differentiator, str, status);
        if (tree_node != NULL)
            return tree_node;

        tree_node = LangGetFunction(differentiator, str, status);
        if (tree_node != NULL)
            return tree_node;
    }

    DUMP_CURRENT_SITUATION(tree_node);

    return tree_node;
}

Tree_node* LangGetNumber(Differentiator* differentiator, char** str, Tree_status* status) {
    assert(str);
    assert(*str);

    char* old_str = *str;
    double val = 0;

    Tree_node* tree_node = NULL;

    while ('0' <= **str && **str <= '9') {
        val = val * 10 + (**str - '0');
        (*str)++;
    }

    if (old_str == *str)
        *status = NOT_NUMBER;

    else 
        tree_node = SpuskNodeCtor(NUMBER, (type_t){.number = val}, NULL, NULL);

    DUMP_CURRENT_SITUATION(tree_node);

    return tree_node;
}

Tree_node* LangGetVariable(Differentiator* differentiator, char** str, Tree_status*) {
    assert(str);
    assert(*str);

    char* first_symbol = *str;

    Tree_node* tree_node = NULL;

    if (('a' <= **str && **str <= 'z') || **str == '_') {
        (*str)++;

        while (('a' <= **str && **str <= 'z') || **str == '_' || ('0' <= **str && **str <= '9'))
            (*str)++;

        char* name_variable = strndup(first_symbol, (size_t)(*str - first_symbol));

        for (size_t i = 0; i < sizeof(about_functions) / sizeof(about_functions[0]); ++i) {
            if (strcmp(name_variable, about_functions[i].name) == 0) {
                *str = first_symbol;
                free(name_variable);
                return NULL;
            }    
        }

        for (size_t i = 0; i < differentiator->array_with_variables.size; ++i) {
            if (strcmp(name_variable, differentiator->array_with_variables.data[i]->name) == 0) {
                free(name_variable);
                return SpuskNodeCtor(VARIABLE, (type_t){.index_variable = (int)i}, NULL, NULL);
            }
        }
    
        About_variable* about_variable = (About_variable*)calloc(1, sizeof(About_variable));
        *about_variable = {.name = name_variable, .value = DEFAULT_VALUE};

        ArrayPushvariables(&differentiator->array_with_variables, about_variable);

        return SpuskNodeCtor(VARIABLE, (type_t){.index_variable = (int)(differentiator->array_with_variables.size - 1)}, NULL, NULL);
    }

    return tree_node;
}

Tree_node* LangGetFunction(Differentiator* differentiator, char** str, Tree_status* status) {
    assert(differentiator);
    assert(str);

    for (size_t i = 0; i < sizeof(about_functions) / sizeof(about_functions[0]); ++i) {
        size_t len = strlen(about_functions[i].name);
        if (strncmp(about_functions[i].name, *str, len) == 0) {
            *str += len;

            if (**str == '(') {
                (*str)++;

                Tree_node* tree_node = LangGetExpression(differentiator, str, status);

                if (**str == ')') {
                    (*str)++;
                    return SpuskNodeCtor(OPERATOR, (type_t){.operators = about_functions[i].type}, tree_node, NULL);
                }

                else {
                    *status = NOT_END_SKOBKA;
                }
            }
        }
    }

    return NULL;
}

Tree_node* SpuskNodeCtor(Type_node type, type_t value,
                    Tree_node* left_node, Tree_node* right_node) {

    Tree_node* new_node = (Tree_node*)calloc(1, sizeof(Tree_node));
    if (new_node == NULL)
        return NULL;

    new_node->left_node  = left_node;
    new_node->right_node = right_node;
    new_node->type       = type;
    new_node->value      = value;

    return new_node;
}

int SpuskSizeOfText(const char *text_name) {
    assert(text_name);

    struct stat text_info = {};

    if (stat(text_name, &text_info) == -1) {
        perror("Error is");

        return -1;
    }

    return (int)text_info.st_size;
}

Tree_status SpuskReadOnegin(char** str, const char* name_file) {
    assert(name_file != NULL);
    assert(str);    

    FILE *text = fopen(name_file, "r");
    if (text == NULL) {
        perror("Error is");
        return OPEN_ERROR;
    }

    int size = SpuskSizeOfText(name_file);
    if (size == -1) {
        return STAT_ERROR;
    }

    *str = (char*)calloc((size_t)size + 1, sizeof(char));
    if (*str == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(MEMORY_ERROR);

    fread((char *)*str, sizeof(char), (size_t)size, text);
    if (ferror(text) != 0) {
        TREE_CHECK_AND_RETURN_ERRORS(READ_ERROR,    free(*str));
    }

    *(*str + size) = '\0';

    return SUCCESS;
}