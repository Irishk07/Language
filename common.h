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
    OPERATOR_MATCH         = 35,
    OPERATOR_DEF_FUNCTION  = 36,
    OPERATOR_CALL_FUNCTION = 37,
    OPERATOR_RETURN        = 38,
    OPERATOR_PARAM         = 39,
    OPERATOR_MAIN_FUNCTION = 40,
    WRONG_OPERATOR
};

struct About_function {
    const char* name;
    Type_operators type;
    unsigned long hash;
    const char* tree_name;
    unsigned long tree_hash;
};
 
unsigned long hash_djb2(const char *str);


#define KEY_WORD(command_name, command_type, command_tree_name)                   \
    {.name = command_name, .type = command_type, .hash = hash_djb2(command_name), \
     .tree_name = command_tree_name, .tree_hash = hash_djb2(command_tree_name)}

About_function const key_words[] = {
    KEY_WORD("<3",     OPERATOR_COMMON,        ";"),
    KEY_WORD("*_^",    OPERATOR_OPEN_BRACKET,  "("),
    KEY_WORD("^_*",    OPERATOR_CLOSE_BRACKET, ")"),
    KEY_WORD("+___-",  OPERATOR_OPEN_FIGURE,   "{"),
    KEY_WORD("-___+",  OPERATOR_CLOSE_FIGURE,  "}"),
    KEY_WORD("->",     OPERATOR_MATCH,         "->"),
    KEY_WORD("+",      OPERATOR_ADD,           "+"),
    KEY_WORD("-",      OPERATOR_SUB,           "-"),
    KEY_WORD("/",      OPERATOR_DIV,           "/"),
    KEY_WORD("*",      OPERATOR_MUL,           "*"),
    KEY_WORD("^",      OPERATOR_POW,           "^"),
    KEY_WORD("<",      OPERATOR_BEFORE,        "<"),
    KEY_WORD(">",      OPERATOR_ABOVE,         ">"),
    KEY_WORD("==",     OPERATOR_EQUAL,         "=="),
    KEY_WORD("ln",     OPERATOR_LN,            "ln"),
    KEY_WORD("log",    OPERATOR_LOG,           "log"),
    KEY_WORD("sin",    OPERATOR_SIN,           "sin"),
    KEY_WORD("cos",    OPERATOR_COS,           "cos"),
    KEY_WORD("tg",     OPERATOR_TG,            "tg"),
    KEY_WORD("ctg",    OPERATOR_CTG,           "ctg"),
    KEY_WORD("arcsin", OPERATOR_ARCSIN,        "arcsin"),
    KEY_WORD("arccos", OPERATOR_ARCCOS,        "arccos"),
    KEY_WORD("arctg",  OPERATOR_ARCTG,         "arctg"),
    KEY_WORD("arcctg", OPERATOR_ARCCTG,        "arcctg"),
    KEY_WORD("sh",     OPERATOR_SH,            "sh"),
    KEY_WORD("ch",     OPERATOR_CH,            "ch"),
    KEY_WORD("th",     OPERATOR_TH,            "th"),
    KEY_WORD("cth",    OPERATOR_CTH,           "cth"),
    KEY_WORD("Bye",    OPERATOR_FINISH_SYMBOL, "$"),
    KEY_WORD("and",    OPERATOR_PARAM,         ","),
    KEY_WORD("We resolve conflict if you call chef and check", OPERATOR_IF,            "if"),
    KEY_WORD("We won't resolve conflict. But",                 OPERATOR_ELSE,          "else"),
    KEY_WORD("I didn't write complaint only because",          OPERATOR_WHILE,         "while"),
    KEY_WORD("Bring menu",                                     OPERATOR_INPUT,         "input"),
    KEY_WORD("Calculate how much dish",                        OPERATOR_PRINT,         "print"),
    KEY_WORD("I want to place an order:",                      OPERATOR_ASSIGNMENT,    ":="),
    KEY_WORD("I repeat my order:",                             OPERATOR_ASSIGNMENT,    ":="),
    KEY_WORD("I want to change my order:",                     OPERATOR_CHANGE,        "="),
    KEY_WORD("I repeat changes:",                              OPERATOR_CHANGE,        "="),
    KEY_WORD("Recipe:",                                        OPERATOR_DEF_FUNCTION,  "func"),
    KEY_WORD("Welcome!",                                       OPERATOR_MAIN_FUNCTION, "main"),
    KEY_WORD("Waiter!",                                        OPERATOR_CALL_FUNCTION, "call"),
    KEY_WORD("Return the money for",                           OPERATOR_RETURN,        "return")
};

#undef KEY_WORD


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