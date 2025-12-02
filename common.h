#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>

#define TREE_CHECK_AND_RETURN_ERRORS(error, ...)                          \
        {                                                                 \
            Tree_status now_error = (error);                              \
            if (now_error != SUCCESS) {                                   \
                fprintf(stderr, "Error is: %d, %d\n", (error), __LINE__); \
                __VA_ARGS__;                                              \
                return now_error;                                         \
            }                                                             \
        }


const int CNT_ATTEMPTS  = 5;
const int DEFAULT_VALUE = 666;


enum Type_node {
    VARIABLE    = 0,
    NUMBER      = 1,
    OPERATOR    = 2,
    WRONG_TYPE  = 3
};

enum Type_operators {
    OPERATOR_ADD    = 0,
    OPERATOR_SUB    = 1,
    OPERATOR_MUL    = 2,
    OPERATOR_DIV    = 3,
    OPERATOR_POW    = 4,
    OPERATOR_LN     = 5,
    OPERATOR_LOG    = 6,
    OPERATOR_SIN    = 7,
    OPERATOR_COS    = 8,
    OPERATOR_TG     = 9,
    OPERATOR_CTG    = 10,
    OPERATOR_ARCSIN = 11,
    OPERATOR_ARCCOS = 12,
    OPERATOR_ARCTG  = 13,
    OPERATOR_ARCCTG = 14,
    OPERATOR_SH     = 15,  
    OPERATOR_CH     = 16,  
    OPERATOR_TH     = 17,  
    OPERATOR_CTH    = 18, 
    OPERATOR_EQUAL  = 19,
    OPERATOR_PC     = 20,
    OPERATOR_IF     = 21,
    WRONG_OPERATOR
};

struct About_function {
    const char* name;
    Type_operators type;
};

About_function const about_functions[] = {{.name = "ln", .type = OPERATOR_LN}, 
                                          {.name = "log", .type = OPERATOR_LOG},
                                          {.name = "sin", .type = OPERATOR_SIN},
                                          {.name = "cos", .type = OPERATOR_COS},
                                          {.name = "tg", .type = OPERATOR_TG},
                                          {.name = "ctg", .type = OPERATOR_CTG},
                                          {.name = "arcsin", .type = OPERATOR_ARCSIN},
                                          {.name = "arccos", .type = OPERATOR_ARCCOS},
                                          {.name = "arctg", .type = OPERATOR_ARCTG},
                                          {.name = "arcctg", .type = OPERATOR_ARCCTG},
                                          {.name = "sh", .type = OPERATOR_SH},
                                          {.name = "ch", .type = OPERATOR_CH},
                                          {.name = "th", .type = OPERATOR_TH},
                                          {.name = "cth", .type = OPERATOR_CTH}};


union type_t {
    double number;
    int    index_variable;
    Type_operators operators;
};

enum Tree_status {
    SUCCESS                  = 0,
    MEMORY_ERROR             = 1,
    OPEN_ERROR               = 2,
    EXECUTION_FAILED         = 3,
    CLOSE_ERROR              = 4,
    WRONG_SITUATION          = 5,
    READ_ERROR               = 6,
    NULL_POINTER_ON_TREE     = 7,
    WRONG_SIZE               = 8,
    NULL_POINTER_ON_NODE     = 9,
    PARENT_AND_CHILD_UNEQUAL = 10,
    WRONG_ROOT               = 11,
    WRONG_NODE               = 12,
    NULL_POINTER_ON_LEAF     = 13,
    SYNTAX_ERROR             = 14,
    BUFFER_OVERFLOW          = 15,
    STAT_ERROR               = 16,
    INPUT_ERROR              = 17,
    NOT_END_SYMBOL           = 18,
    NOT_END_SKOBKA           = 19,
    NOT_NUMBER               = 20
};


struct Tree_node {
    Type_node type;
    type_t value;
    Tree_node* left_node;
    Tree_node* right_node;
};

struct Dump_information {
    const char* html_dump_filename;
    const char* directory;
    int num_html_dump = 0;
    FILE* tex_dump_file;
};

struct Tree {
    Tree_node* root;
    size_t size;
};

struct About_tree {
    Tree* tree;
    double value;
};

typedef About_tree* trees_type_t;

struct About_variable {
    char* name;
    double value;
};

typedef About_variable* variables_type_t;

struct Array_with_variables {
    variables_type_t* data = NULL;
    size_t size = 0;
    size_t capacity = 0;
};

struct Array_with_trees {
    trees_type_t* data = NULL;
    size_t size = 0;
    size_t capacity = 0;
};

struct Differentiator {
    Tree tree;
    Array_with_variables array_with_variables;
    char* begin_buffer;
    char* end_buffer;
    size_t size_buffer;
    char* buffer_with_tree = NULL;
    Dump_information dump_info;
};


#endif // COMMON_H_