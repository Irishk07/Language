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


#define DUMP_INFO __LINE__, __FILE__
#define NOT_ERROR_DUMP USUAL_DUMP, SUCCESS

#ifdef REVERSE
#define IF_REVERSE(...) __VA_ARGS__
#else
#define IF_REVERSE(...)
#endif // REVERSE


const int CNT_ATTEMPTS  = 5;
const int DEFAULT_VALUE = 666;


enum Type_node {
    VARIABLE    = 0,
    NUMBER      = 1,
    OPERATOR    = 2,
    WRONG_TYPE  = 3
};

enum Type_operators {
    OPERATOR_COMMON        = 0,
    OPERATOR_OPEN_BRACKET  = 1,
    OPERATOR_CLOSE_BRACKET = 2,
    OPERATOR_OPEN_FIGURE   = 3,
    OPERATOR_CLOSE_FIGURE  = 4,
    OPERATOR_MATCH         = 5,
    OPERATOR_ADD           = 6,
    OPERATOR_SUB           = 7,
    OPERATOR_DIV           = 8,
    OPERATOR_MUL           = 9,
    OPERATOR_POW           = 10,
    OPERATOR_BEFORE        = 11,
    OPERATOR_ABOVE         = 12,
    OPERATOR_EQUAL         = 13,
    OPERATOR_LN            = 14,
    OPERATOR_LOG           = 15,  
    OPERATOR_SIN           = 16,  
    OPERATOR_COS           = 17,  
    OPERATOR_TG            = 18, 
    OPERATOR_CTG           = 19,
    OPERATOR_ARCSIN        = 20,
    OPERATOR_ARCCOS        = 21,
    OPERATOR_ARCTG         = 22,
    OPERATOR_ARCCTG        = 23,
    OPERATOR_SH            = 24,
    OPERATOR_CH            = 25,
    OPERATOR_TH            = 26,
    OPERATOR_CTH           = 27,
    OPERATOR_SQRT          = 28,
    OPERATOR_FINISH_SYMBOL = 29,
    OPERATOR_PARAM         = 30,
    OPERATOR_IF            = 31,
    OPERATOR_ELSE          = 32,
    OPERATOR_WHILE         = 33,
    OPERATOR_INPUT         = 34,
    OPERATOR_PRINT         = 35,
    OPERATOR_ASSIGNMENT    = 36,
    OPERATOR_C_ASSIGNMENT  = 37,
    OPERATOR_CHANGE        = 38,
    OPERATOR_C_CHANGE      = 39,
    OPERATOR_DEF_FUNCTION  = 40,
    OPERATOR_MAIN_FUNCTION = 41,
    OPERATOR_CALL_FUNCTION = 42,
    OPERATOR_RETURN        = 43,
    OPERATOR_DRAW          = 44,
    WRONG_OPERATOR
};

struct Language;
struct Tree_node;

struct About_function {
    const char* name;
    Type_operators type;
    unsigned long hash;
    const char* tree_name;
    unsigned long tree_hash;
    IF_REVERSE(void (*function) (Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs));
};

unsigned long hash_djb2(const char *str);

void do_math_operations(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs);
void do_math_functions(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs);
void do_if(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs);
void do_else(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs);
void do_while(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs);
void do_print(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs);
void do_input(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs);
void do_draw(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs);
void do_assignment_change(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs);
void do_def_func(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs);
void do_main_func(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs);
void do_call_func(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs);
void do_return(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs);

#define KEY_WORD(command_name, command_type, command_tree_name, command_function) \
    {.name = command_name, .type = command_type, .hash = hash_djb2(command_name), \
     .tree_name = command_tree_name, .tree_hash = hash_djb2(command_tree_name)    \
     IF_REVERSE(, .function = command_function)}

About_function const key_words[] = {
    KEY_WORD("<3",     OPERATOR_COMMON,        ";", NULL),
    KEY_WORD("*_^",    OPERATOR_OPEN_BRACKET,  "(", NULL),
    KEY_WORD("^_*",    OPERATOR_CLOSE_BRACKET, ")", NULL),
    KEY_WORD("+___-",  OPERATOR_OPEN_FIGURE,   "{", NULL),
    KEY_WORD("-___+",  OPERATOR_CLOSE_FIGURE,  "}", NULL),
    KEY_WORD("->",     OPERATOR_MATCH,         "->", NULL),
    KEY_WORD("+",      OPERATOR_ADD,           "+", &do_math_operations),
    KEY_WORD("-",      OPERATOR_SUB,           "-", &do_math_operations),
    KEY_WORD("/",      OPERATOR_DIV,           "/", &do_math_operations),
    KEY_WORD("*",      OPERATOR_MUL,           "*", &do_math_operations),
    KEY_WORD("^",      OPERATOR_POW,           "^", &do_math_operations),
    KEY_WORD("<",      OPERATOR_BEFORE,        "<", &do_math_operations),
    KEY_WORD(">",      OPERATOR_ABOVE,         ">", &do_math_operations),
    KEY_WORD("==",     OPERATOR_EQUAL,         "==", &do_math_operations),
    KEY_WORD("ln",     OPERATOR_LN,            "ln", &do_math_functions),
    KEY_WORD("log",    OPERATOR_LOG,           "log", &do_math_functions),
    KEY_WORD("sin",    OPERATOR_SIN,           "sin", &do_math_functions),
    KEY_WORD("cos",    OPERATOR_COS,           "cos", &do_math_functions),
    KEY_WORD("tg",     OPERATOR_TG,            "tg", &do_math_functions),
    KEY_WORD("ctg",    OPERATOR_CTG,           "ctg", &do_math_functions),
    KEY_WORD("arcsin", OPERATOR_ARCSIN,        "arcsin", &do_math_functions),
    KEY_WORD("arccos", OPERATOR_ARCCOS,        "arccos", &do_math_functions),
    KEY_WORD("arctg",  OPERATOR_ARCTG,         "arctg", &do_math_functions),
    KEY_WORD("arcctg", OPERATOR_ARCCTG,        "arcctg", &do_math_functions),
    KEY_WORD("sh",     OPERATOR_SH,            "sh", &do_math_functions),
    KEY_WORD("ch",     OPERATOR_CH,            "ch", &do_math_functions),
    KEY_WORD("th",     OPERATOR_TH,            "th", &do_math_functions),
    KEY_WORD("cth",    OPERATOR_CTH,           "cth", &do_math_functions),
    KEY_WORD("sqrt",   OPERATOR_SQRT,          "sqrt", &do_math_functions),
    KEY_WORD("Bye",    OPERATOR_FINISH_SYMBOL, "$", NULL),
    KEY_WORD("and",    OPERATOR_PARAM,         ",", NULL),
    KEY_WORD("We resolve conflict if you call chef and check", OPERATOR_IF,            "if", &do_if),
    KEY_WORD("We won't resolve conflict. But",                 OPERATOR_ELSE,          "else", &do_else),
    KEY_WORD("I didn't write complaint only because",          OPERATOR_WHILE,         "while", &do_while),
    KEY_WORD("Bring menu",                                     OPERATOR_INPUT,         "input", &do_input),
    KEY_WORD("Calculate how much dish",                        OPERATOR_PRINT,         "print", &do_print),
    KEY_WORD("I want to place an order:",                      OPERATOR_ASSIGNMENT,    ":=", &do_assignment_change),
    KEY_WORD("I repeat my order:",                             OPERATOR_C_ASSIGNMENT,  ":=", &do_assignment_change),
    KEY_WORD("I want to change my order:",                     OPERATOR_CHANGE,        "=", &do_assignment_change),
    KEY_WORD("I repeat changes:",                              OPERATOR_C_CHANGE,      "=", &do_assignment_change),
    KEY_WORD("Recipe:",                                        OPERATOR_DEF_FUNCTION,  "func", &do_def_func),
    KEY_WORD("Welcome!",                                       OPERATOR_MAIN_FUNCTION, "main", &do_main_func),
    KEY_WORD("Waiter!",                                        OPERATOR_CALL_FUNCTION, "call", &do_call_func),
    KEY_WORD("Return the money for",                           OPERATOR_RETURN,        "return", &do_return),
    KEY_WORD("I want to drawdrawdraw",                         OPERATOR_DRAW,          "draw", &do_draw)
};

#undef KEY_WORD


struct About_variable {
    const char* name;
    int value;
};

struct Array_with_data {
    void* data = NULL;
    size_t size = 0;
    size_t capacity = 0;
    size_t elem_size;
};

union type_t {
    int number;
    About_variable* about_variable;
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



char* ReadAnswer();

int ValueOfVariable(Tree_node* tree_node);

int ValueOfVariableFromIndex(Array_with_data* array_with_variables, size_t index);

const char* NameOfVariable(Tree_node* tree_node);

const char* NameOfVariableFromIndex(Array_with_data* array_with_variables, size_t index);

const char* IndetifySign(Tree_node* tree_node);

void SkipSpaces(char** buffer);

void SkipComments(char** buffer);

Type_node FindOutType(const char* name, type_t* value);

Status_of_finding ItIsOperator(const char* name, type_t* value);

Status_of_finding ItIsNumber(const char* name, type_t* value);

Status_of_finding ItIsVariable(const char* name, type_t* value);


#endif // COMMON_H_