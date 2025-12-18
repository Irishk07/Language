#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "common.h"


char* ReadAnswer() {
    char* answer = NULL;
    size_t size_buf = 0;

    if (getline(&answer, &size_buf, stdin) == -1)
        return NULL;

    answer[size_buf - 1] = '\0'; 

    return answer;
}

int ValueOfVariable(Tree_node* tree_node) {
    assert(tree_node);

    return tree_node->value.about_variable->value;
}

int ValueOfVariableFromIndex(Array_with_data* array_with_variables, size_t index) {
    assert(array_with_variables);

    About_variable about_variable = {};
    ArrayGetElement(array_with_variables, &about_variable, index);

    return about_variable.value;
}

const char* NameOfVariable(Tree_node* tree_node) {
    assert(tree_node);

    return tree_node->value.about_variable->name;
}

const char* NameOfVariableFromIndex(Array_with_data* array_with_variables, size_t index) {
    assert(array_with_variables);

    About_variable about_variable = {};
    ArrayGetElement(array_with_variables, &about_variable, index);

    return about_variable.name;
}

const char* IndetifySign(Tree_node* tree_node) {
    assert(tree_node);

    switch(tree_node->type) {
        case OPERATOR:
            switch (tree_node->value.operators) {
                case OPERATOR_ADD:           return "+";
                case OPERATOR_SUB:           return "-";
                case OPERATOR_MUL:           return "*";
                case OPERATOR_DIV:           return "/";
                case OPERATOR_POW:           return "^";
                case OPERATOR_LN:            return "ln";
                case OPERATOR_LOG:           return "log";
                case OPERATOR_SIN:           return "sin";
                case OPERATOR_COS:           return "cos";
                case OPERATOR_TG:            return "tg";
                case OPERATOR_CTG:           return "ctg";
                case OPERATOR_ARCSIN:        return "arcsin";
                case OPERATOR_ARCCOS:        return "arccos";
                case OPERATOR_ARCTG:         return "arctg";
                case OPERATOR_ARCCTG:        return "arcctg";
                case OPERATOR_SH:            return "sh";
                case OPERATOR_CH:            return "ch";
                case OPERATOR_TH:            return "th";
                case OPERATOR_CTH:           return "cth";
                case OPERATOR_ASSIGNMENT:    return ":=";
                case OPERATOR_CHANGE:        return "=";
                case OPERATOR_COMMON:        return ";";
                case OPERATOR_IF:            return "if";
                case OPERATOR_WHILE:         return "while";
                case OPERATOR_ELSE:          return "else";
                case OPERATOR_INPUT:         return "input";
                case OPERATOR_PRINT:         return "print";
                case OPERATOR_ABOVE:         return ">";
                case OPERATOR_BEFORE:        return "<";
                case OPERATOR_EQUAL:         return "==";
                case OPERATOR_OPEN_BRACKET:  return "(";
                case OPERATOR_CLOSE_BRACKET: return ")";
                case OPERATOR_OPEN_FIGURE:   return "{";
                case OPERATOR_CLOSE_FIGURE:  return "}";
                case OPERATOR_FINISH_SYMBOL: return "$";
                case OPERATOR_MATCH:         return "->";
                case OPERATOR_DEF_FUNCTION:  return "func";
                case OPERATOR_MAIN_FUNCTION: return "main";
                case OPERATOR_CALL_FUNCTION: return "call";
                case OPERATOR_RETURN:        return "return";
                case OPERATOR_PARAM:         return ",";
                case OPERATOR_SQRT:          return "sqrt";
                case WRONG_OPERATOR:
                default: break;
            }
        case NUMBER:
        case VARIABLE:
        case WRONG_TYPE:
        default: break;
    }

    return NULL;
}

void SkipSpaces(char** buffer) {
    assert(buffer);
    assert(*buffer);

    while (isspace(**buffer) && (**buffer) != '\0')
        (*buffer)++;
}

void SkipComments(char** buffer) {
    assert(buffer);
    assert(*buffer);

    if (**buffer != '#')
        return;
    
    (*buffer)++;

    while (**buffer != '\n' && (**buffer) != '\0')
        (*buffer)++;

    (*buffer)++;
}

unsigned long hash_djb2(const char *str) {
    assert(str);

    unsigned long hash = 5381;
    int c = 0;

    while ((c = *(str++)) != '\0') {
        hash = ((hash << 5) + hash) + (unsigned long)c; /* hash * 33 + c */
    }

    return hash;
}

Type_node FindOutType(const char* name, type_t* value) {
    assert(name);
    assert(value);

    if (ItIsOperator(name, value) == FIND_YES)
        return OPERATOR;

    if (ItIsNumber(name, value) == FIND_YES)
        return NUMBER;

    if (ItIsVariable(name, value) == FIND_YES)
        return VARIABLE;

    return WRONG_TYPE;
}

Status_of_finding ItIsOperator(const char* name, type_t* value) {
    assert(name);
    assert(value);

    unsigned long hash = hash_djb2(name);

    for (size_t i = 0; i < sizeof(key_words) / sizeof(key_words[0]); ++i) {
        if (hash == key_words[i].tree_hash && strcmp(name, key_words[i].tree_name) == 0) {
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

Status_of_finding ItIsVariable(const char* name, type_t* value) {
    assert(name);
    assert(value);

    if (isdigit(*name))
        return FIND_NO;

    About_variable* about_variable = (About_variable*)calloc(1, sizeof(About_variable));
    about_variable->name  = name;
    about_variable->value = DEFAULT_VALUE;

    value->about_variable = about_variable;
        
    return FIND_YES;
}