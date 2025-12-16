#ifndef SYNTAX_ANALIZE_H_
#define SYNTAX_ANALIZE_H_

#include "../common.h"
#include "front_end.h"


enum Status_of_comparing {
    EQUAL     = 0,
    DIFFERENT = 1
};


Tree_node* LangGetComandir(Language* language, Tree_status* status);

Tree_node* LangGetOperators(Language* language, size_t* number_token, Tree_status* status);

Tree_node* LangGetWhileOrIf(Language* language, size_t* number_token, Tree_status* status);

Tree_node* LangGetElse(Language* language, size_t* number_token, Tree_status* status, Tree_node* tree_node_operator);

Tree_node* LangGetCompares(Language* language, size_t* number_token, Tree_status* status);

Tree_node* LangGetEquals(Language* language, size_t* number_token, Tree_status* status);

Tree_node* CheckEqualAssignmentOrChange(Language* language, size_t* number_token, Tree_status* status);

Tree_node* LangGetAssignmentOrChange(Language* language, size_t* number_token, Tree_status* status);

Status_of_comparing CompareTrees(Tree_node* tree_node_1, Tree_node* tree_node_2);

Tree_node* LangGetExpression(Language* language, size_t* number_token, Tree_status* status);

Tree_node* LangGetTerm(Language* language, size_t* number_token, Tree_status* status);

Tree_node* LangGetPow(Language* language, size_t* number_token, Tree_status* status);

Tree_node* LangGetPrimaryExpression(Language* language, size_t* number_token, Tree_status* status);

Tree_node* LangGetNumber(Language* language, size_t* number_token, Tree_status* status);

Tree_node* LangGetVariable(Language* language, size_t* number_token, Tree_status*);

Tree_node* LangGetMathFunction(Language* language, size_t* number_token, Tree_status* status);

Tree_node* LangGetInput(Language* language, size_t* number_token, Tree_status* status);

Tree_node* LangGetPrint(Language* language, size_t* number_token, Tree_status* status);


#endif // SYNTAX_ANALIZE_H_