#ifndef LANGUAGE_H_
#define LANGUAGE_H_

#include "common.h"


Tree_status LanguageCtor(Language* language, const char* html_dump_filename, const char* directory);

Tree_status LanguageDtor(Language* language);

void LanguageNodeDtor(Language* language, Tree_node* tree_node);

void ArrayDtorVariables(Language* language, Array_with_data* array_with_data);

void ArrayDtorTokens(Language* language, Array_with_data* array_with_data);


#endif // LANGUAGE_H_