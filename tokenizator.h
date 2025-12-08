#ifndef TOKENIZATOR_H_
#define TOKENIZATOR_H_

#include "common.h"

Tree_status Tokenizator(Language* language, const char* file_name);

Tree_node* ReadNumber(char** str);

Tree_node* ReadVariable(Language* language, char** str);

Tree_node* CheckKeyWords(char* name_variable, unsigned long hash_variable);

Tree_node* CheckSigns(Language* language, char** str);

#endif // TOKENIZATOR_H_