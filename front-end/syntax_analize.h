#ifndef SYNTAX_ANALIZE_H_
#define SYNTAX_ANALIZE_H_

#include "../common.h"
#include "language.h"


Tree_node* LangGetComandir(Language* language, Tree_status* status);

Tree_node* LangGetOperators(Language* language, size_t* number_token, Tree_status* status);

Tree_node* LangGetWhileOrIf(Language* language, size_t* number_token, Tree_status* status);

Tree_node* LangGetElse(Language* language, size_t* number_token, Tree_status* status, Tree_node* tree_node_operator);

Tree_node* LangGetAssignment(Language* language, size_t* number_token, Tree_status* status);

Tree_node* LangGetExpression(Language* language, size_t* number_token, Tree_status* status);

Tree_node* LangGetTerm(Language* language, size_t* number_token, Tree_status* status);

Tree_node* LangGetPow(Language* language, size_t* number_token, Tree_status* status);

Tree_node* LangGetPrimaryExpression(Language* language, size_t* number_token, Tree_status* status);

Tree_node* LangGetNumber(Language* language, size_t* number_token, Tree_status* status);

Tree_node* LangGetVariable(Language* language, size_t* number_token, Tree_status*);

Tree_node* LangGetMathFunction(Language* language, size_t* number_token, Tree_status* status);

#endif // SYNTAX_ANALIZE_H_