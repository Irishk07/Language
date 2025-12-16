#ifndef TOKENIZATOR_H_
#define TOKENIZATOR_H_

#include "../common.h"
#include "front_end.h"

About_function const prog_key_words[] = {
    {.name = "<3",     .type = OPERATOR_COMMON,        .hash = hash_djb2("<3")},
    {.name = "*_^",    .type = OPERATOR_OPEN_BRACKET,  .hash = hash_djb2("*_^")},
    {.name = "^_*",    .type = OPERATOR_CLOSE_BRACKET, .hash = hash_djb2("^_*")},
    {.name = "+___-",  .type = OPERATOR_OPEN_FIGURE,   .hash = hash_djb2("+___-")},
    {.name = "-___+",  .type = OPERATOR_CLOSE_FIGURE,  .hash = hash_djb2("-___+")},
    {.name = "->",     .type = OPERATOR_MATCH,         .hash = hash_djb2("->")},
    {.name = "+",      .type = OPERATOR_ADD,           .hash = hash_djb2("+")},
    {.name = "-",      .type = OPERATOR_SUB,           .hash = hash_djb2("-")}, 
    {.name = "/",      .type = OPERATOR_DIV,           .hash = hash_djb2("/")},
    {.name = "*",      .type = OPERATOR_MUL,           .hash = hash_djb2("*")},
    {.name = "^",      .type = OPERATOR_POW,           .hash = hash_djb2("^")},
    {.name = "<",      .type = OPERATOR_BEFORE,        .hash = hash_djb2("<")},
    {.name = ">",      .type = OPERATOR_ABOVE,         .hash = hash_djb2(">")},
    {.name = "==",     .type = OPERATOR_EQUAL,         .hash = hash_djb2("==")},
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
    {.name = "Bye",    .type = OPERATOR_FINISH_SYMBOL, .hash = hash_djb2("Bye")},
    {.name = "We resolve conflict if you call chef and check", .type = OPERATOR_IF,         .hash = hash_djb2("We resolve conflict if you call chef and check")},
    {.name = "We won't resolve conflict. But",                 .type = OPERATOR_ELSE,       .hash = hash_djb2("We won't resolve conflict. But")},
    {.name = "I didn't write complaint only because",          .type = OPERATOR_WHILE,      .hash = hash_djb2("I didn't write complaint only because")},
    {.name = "Bring menu",                                     .type = OPERATOR_INPUT,      .hash = hash_djb2("Bring menu")},
    {.name = "Calculate how much dish",                        .type = OPERATOR_PRINT,      .hash = hash_djb2("Calculate how much")},
    {.name = "I want to change my order:",                     .type = OPERATOR_CHANGE,     .hash = hash_djb2("I want to change my order:")},
    {.name = "I repeat changes:",                              .type = OPERATOR_CHANGE,     .hash = hash_djb2("I repeat changes:")},
    {.name = "I want to place an order: bring to me",          .type = OPERATOR_ASSIGNMENT, .hash = hash_djb2("I want to place an order: bring to me")},
    {.name = "I repeat my order:",                             .type = OPERATOR_ASSIGNMENT, .hash = hash_djb2("I repeat my order:")},
};



Tree_status Tokenizator(Language* language);

Tree_node* ReadNumber(char** str);

Tree_node* ReadVariable(Language* language, char** str);

Tree_node* CheckKeyWords(char* name_variable, unsigned long hash_variable);

Tree_node* ReadKeyWords(Language* language, char** str);


#endif // TOKENIZATOR_H_