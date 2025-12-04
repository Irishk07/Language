#ifndef START_LANG_H_
#define START_LANG_H_

#include "common.h"


Tree_node* LangGetComandir(Differentiator* differentiator, Tree_status* status, const char* file_name);

Tree_node* LangGetOperators(Differentiator* differentiator, char** str, Tree_status* status);

Tree_node* LangGetIf(Differentiator* differentiator, char** str, Tree_status* status);

Tree_node* LangGetAssignment(Differentiator* differentiator, char** str, Tree_status* status);

Tree_node* LangGetExpression(Differentiator* differentiator, char** str, Tree_status* status);

Tree_node* LangGetTerm(Differentiator* differentiator, char** str, Tree_status* status);

Tree_node* LangGetPow(Differentiator* differentiator, char** str, Tree_status* status);

Tree_node* LangGetPrimaryExpression(Differentiator* differentiator, char** str, Tree_status* status);

Tree_node* LangGetNumber(Differentiator*, char** str, Tree_status* status);

Tree_node* LangGetVariable(Differentiator* differentiator, char** str, Tree_status*);

Tree_node* LangGetFunction(Differentiator* differentiator, char** str, Tree_status* status);

Tree_node* SpuskNodeCtor(Type_node type, type_t value,
                    Tree_node* left_node, Tree_node* right_node);

int SpuskSizeOfText(const char *text_name);

Tree_status SpuskReadOnegin(char** str, const char* name_file);

#endif // START_LANG_H_