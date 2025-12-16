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

#define DUMP_CURRENT_SITUATION(node)                              \
{                                                                 \
        TreeHTMLDump(language, node, DUMP_INFO, NOT_ERROR_DUMP);  \
}

#define DUMP_INFO __LINE__, __FILE__
#define NOT_ERROR_DUMP USUAL_DUMP, SUCCESS


const int CNT_ATTEMPTS  = 5;
const int DEFAULT_VALUE = 666;


enum Type_node {
    VARIABLE    = 0,
    NUMBER      = 1,
    OPERATOR    = 2,
    WRONG_TYPE  = 3
};

enum Type_operators {
    OPERATOR_ADD           = 0,
    OPERATOR_SUB           = 1,
    OPERATOR_MUL           = 2,
    OPERATOR_DIV           = 3,
    OPERATOR_POW           = 4,
    OPERATOR_LN            = 5,
    OPERATOR_LOG           = 6,
    OPERATOR_SIN           = 7,
    OPERATOR_COS           = 8,
    OPERATOR_TG            = 9,
    OPERATOR_CTG           = 10,
    OPERATOR_ARCSIN        = 11,
    OPERATOR_ARCCOS        = 12,
    OPERATOR_ARCTG         = 13,
    OPERATOR_ARCCTG        = 14,
    OPERATOR_SH            = 15,  
    OPERATOR_CH            = 16,  
    OPERATOR_TH            = 17,  
    OPERATOR_CTH           = 18, 
    OPERATOR_ASSIGNMENT    = 19,
    OPERATOR_COMMON        = 20,
    OPERATOR_IF            = 21,
    OPERATOR_WHILE         = 22,
    OPERATOR_OPEN_BRACKET  = 23,
    OPERATOR_CLOSE_BRACKET = 24,
    OPERATOR_OPEN_FIGURE   = 25,
    OPERATOR_CLOSE_FIGURE  = 26,
    OPERATOR_FINISH_SYMBOL = 27,
    OPERATOR_ELSE          = 28,
    OPERATOR_CHANGE        = 29,
    OPERATOR_INPUT         = 30,
    OPERATOR_PRINT         = 31,
    OPERATOR_BEFORE        = 32,
    OPERATOR_ABOVE         = 33,
    OPERATOR_EQUAL         = 34,
    WRONG_OPERATOR
};

struct About_function {
    const char* name;
    Type_operators type;
    unsigned long hash;
};
 
unsigned long hash_djb2(const char *str);

About_function const key_words[] = {
    {.name = "ln",     .type = OPERATOR_LN,            .hash = hash_djb2("ln")}, 
    {.name = "log",    .type = OPERATOR_LOG,           .hash = hash_djb2("log")},
    {.name = "sin",    .type = OPERATOR_SIN,           .hash = hash_djb2("sin")},
    {.name = "cos",    .type = OPERATOR_COS,           .hash = hash_djb2("cos")},
    {.name = "tg",     .type = OPERATOR_TG,            .hash = hash_djb2("tg")},
    {.name = "ctg",    .type = OPERATOR_CTG,           .hash = hash_djb2("ctg")},
    {.name = "arcsin", .type = OPERATOR_ARCSIN,        .hash = hash_djb2("arcsin")},
    {.name = "arccos", .type = OPERATOR_ARCCOS,        .hash = hash_djb2("arccos")},
    {.name = "arctg",  .type = OPERATOR_ARCTG,         .hash = hash_djb2("arctg")},
    {.name = "arcctg", .type = OPERATOR_ARCCTG,        .hash = hash_djb2("arcctg")},
    {.name = "sh",     .type = OPERATOR_SH,            .hash = hash_djb2("sh")},
    {.name = "ch",     .type = OPERATOR_CH,            .hash = hash_djb2("ch")},
    {.name = "th",     .type = OPERATOR_TH,            .hash = hash_djb2("th")},
    {.name = "cth",    .type = OPERATOR_CTH,           .hash = hash_djb2("cth")},
    {.name = "if",     .type = OPERATOR_IF,            .hash = hash_djb2("if")},
    {.name = "while",  .type = OPERATOR_WHILE,         .hash = hash_djb2("while")},
    {.name = "else",   .type = OPERATOR_ELSE,          .hash = hash_djb2("else")},
    {.name = "input",  .type = OPERATOR_INPUT,         .hash = hash_djb2("input")},
    {.name = "print",  .type = OPERATOR_PRINT,         .hash = hash_djb2("print")},
    {.name = "+",      .type = OPERATOR_ADD,           .hash = hash_djb2("+")},
    {.name = "-",      .type = OPERATOR_SUB,           .hash = hash_djb2("-")}, 
    {.name = "/",      .type = OPERATOR_DIV,           .hash = hash_djb2("/")},
    {.name = "*",      .type = OPERATOR_MUL,           .hash = hash_djb2("*")},
    {.name = "^",      .type = OPERATOR_POW,           .hash = hash_djb2("^")},
    {.name = ";",      .type = OPERATOR_COMMON,        .hash = hash_djb2(";")},
    {.name = "(",      .type = OPERATOR_OPEN_BRACKET,  .hash = hash_djb2("(")},
    {.name = ")",      .type = OPERATOR_CLOSE_BRACKET, .hash = hash_djb2(")")},
    {.name = "{",      .type = OPERATOR_OPEN_FIGURE,   .hash = hash_djb2("{")},
    {.name = "}",      .type = OPERATOR_CLOSE_FIGURE,  .hash = hash_djb2("}")},
    {.name = "=",      .type = OPERATOR_ASSIGNMENT,    .hash = hash_djb2("=")},
    {.name = "$",      .type = OPERATOR_FINISH_SYMBOL, .hash = hash_djb2("$")},
    {.name = ":=",     .type = OPERATOR_CHANGE,        .hash = hash_djb2(":=")},
    {.name = "<",      .type = OPERATOR_BEFORE,        .hash = hash_djb2("<")},
    {.name = ">",      .type = OPERATOR_ABOVE,         .hash = hash_djb2(">")},
    {.name = "==",     .type = OPERATOR_EQUAL,         .hash = hash_djb2("==")}
};

union type_t {
    int number;
    int index_variable;
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
    SYNTAX_ERROR             = 7,
    BUFFER_OVERFLOW          = 8,
    STAT_ERROR               = 9,
    INPUT_ERROR              = 10,
    NOT_END_SYMBOL           = 11,
    NOT_END_SKOBKA           = 12,
    NOT_NUMBER               = 13,
    UNKNOWN_OPERATOR         = 14
};

enum Status_of_finding {
    FIND_NO  = 0,
    FIND_YES = 1
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
};

struct Tree {
    Tree_node* root;
    size_t size;
};

struct About_tree {
    Tree* tree;
    double value;
};
 
struct About_variable {
    char* name;
    int value;
};

struct Array_with_data {
    void* data = NULL;
    size_t size = 0;
    size_t capacity = 0;
    size_t elem_size;
};


#endif // COMMON_H_