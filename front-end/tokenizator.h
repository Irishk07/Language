#ifndef TOKENIZATOR_H_
#define TOKENIZATOR_H_

#include "../common.h"
#include "front_end.h"


Tree_status Tokenizator(Language* language);

Tree_node* ReadNumber(char** str);

Tree_node* ReadVariable(Language* language, char** str);

Tree_node* CheckKeyWords(char* name_variable, unsigned long hash_variable);

Tree_node* ReadSigns(Language* language, char** str);


#endif // TOKENIZATOR_H_