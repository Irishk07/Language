#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "array.h"
#include "common.h"
#include "onegin.h"
#include "syntax_analize.h"
#include "tree.h"

// узел список аргументов - он как узел с точкой запятой

// +++++++++++++++++|||||+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Comandir          ::= Operators+ '$'
// Operators         ::= {Assignment | IF | '{'Operators+'}' } ';'
// IF                ::= "if" '('E')'Operators
// Assignment        ::= Variable'='Expression
// Expression        ::= Term{[+ -]Term}*
// Term              ::= Pow{[* /]Pow}*
// Pow               ::= Assignment{[^]Assignment}*
// PrimaryExpression ::= '('Expression')' | Number | Variable | Function
// Numper            ::= [0-9]+
// Variable          ::= [a-zA-Z_][a-zA-z0-9_]*
// Function          ::= [ln, sin, cos, tg, ctg, arcsin, arccos, arctg, arcctg, sh, ch, th, cth]'('Expression')'
// +++++++++++++++++|||||+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


Tree_node* LangGetComandir(Language* language, Tree_status* status) {
    assert(language);
    assert(status);

    size_t number_token = 0;

    Tree_node* tree_node = NULL;

    do {
        Tree_node* tree_node_2 = LangGetOperators(language, &number_token, status);

        if (tree_node_2 == NULL)
            break;

        if (tree_node != NULL && tree_node->right_node == NULL)
            tree_node->right_node = tree_node_2;
        else
            tree_node = NodeCtor(OPERATOR, (type_t){.operators = OPERATOR_COMMON}, tree_node_2, tree_node);

        TreeHTMLDump(language, tree_node, DUMP_INFO, NOT_ERROR_DUMP);
    }
    while (true);

    Tree_node* cur_token = NULL;
    ArrayGetElement(&(language->array_with_tokens), &cur_token, number_token);
    if ((cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_FINISH_SYMBOL) == 0) {
        *status = NOT_END_SYMBOL;
        return NULL;
    }

    number_token++;

    return tree_node;
}

Tree_node* LangGetOperators(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* tree_node = LangGetIf(language, number_token, status);
    if (tree_node != NULL)
        return tree_node;

    DUMP_CURRENT_SITUATION(tree_node);

    tree_node = LangGetAssignment(language, number_token, status);
    if (tree_node != NULL)
        return tree_node;

    DUMP_CURRENT_SITUATION(tree_node);

    Tree_node* cur_token = NULL;
    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token);
    if (cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_OPEN_FIGURE) {
        (*number_token)++;

        while((cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_OPEN_FIGURE) == 0) {
            Tree_node* tree_node_2 = LangGetOperators(language, number_token, status);

            DUMP_CURRENT_SITUATION(tree_node_2);

            if (tree_node == NULL)
                tree_node = tree_node_2;
            else if (tree_node != NULL && tree_node->right_node == NULL)
                tree_node->right_node = tree_node_2;
            else
                tree_node = NodeCtor(OPERATOR, (type_t){.operators = OPERATOR_COMMON}, tree_node_2, tree_node);

            DUMP_CURRENT_SITUATION(tree_node);

            ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token);
        } 
        
        (*number_token)++;
    }

    DUMP_CURRENT_SITUATION(tree_node);

    return tree_node;
}

Tree_node* LangGetIf(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* cur_token = NULL;
    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token);

    if ((cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_IF) == 0)
        return NULL;

    (*number_token) += 1;

    fprintf(stderr, "%s: HERE\n", __func__);

    TreeHTMLDumpArrayTokens(language, *number_token, DUMP_INFO);

    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token);
    if (cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_OPEN_BRACKET) {
        (*number_token)++;

        fprintf(stderr, "%s: HERE\n", __func__);

        Tree_node* tree_node = LangGetExpression(language, number_token, status);

        DUMP_CURRENT_SITUATION(tree_node);

        ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token);
        if (cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_CLOSE_BRACKET) {
            (*number_token)++;

            Tree_node* tree_node_2 = LangGetOperators(language, number_token, status);

            DUMP_CURRENT_SITUATION(tree_node_2);

            return NodeCtor(OPERATOR, (type_t){.operators = OPERATOR_IF}, tree_node, tree_node_2);
        }

    }
    return NULL;
}

Tree_node* LangGetAssignment(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* tree_node = LangGetVariable(language, number_token, status);
    if (tree_node == NULL)
        return NULL;

    DUMP_CURRENT_SITUATION(tree_node);

    Tree_node* cur_token = NULL;
    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token);

    if (cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_EQUAL) {
        (*number_token)++;

        Tree_node* tree_node_2 = LangGetExpression(language, number_token, status);

        DUMP_CURRENT_SITUATION(tree_node_2);

        ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token);

        if ((cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_COMMON) == 0)
            return NULL;

        (*number_token)++;

        tree_node = NodeCtor(OPERATOR, (type_t){.operators = OPERATOR_EQUAL}, tree_node, tree_node_2);

        DUMP_CURRENT_SITUATION(tree_node);

        return NodeCtor(OPERATOR, (type_t){.operators = OPERATOR_COMMON}, tree_node, NULL);
    }

    return NULL;
}

Tree_node* LangGetExpression(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* new_node = LangGetTerm(language, number_token, status);

    DUMP_CURRENT_SITUATION(new_node);

    Tree_node* cur_token = NULL;
    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token);

    while (cur_token->type == OPERATOR && (cur_token->value.operators == OPERATOR_ADD || cur_token->value.operators == OPERATOR_SUB)) {
        (*number_token)++;

        fprintf(stderr, "%s: HERE\n", __func__);

        Tree_node* new_node_2 = LangGetTerm(language, number_token, status);

        DUMP_CURRENT_SITUATION(new_node_2);

        if (cur_token->value.operators == OPERATOR_ADD)
            new_node = NodeCtor(OPERATOR, (type_t){.operators = OPERATOR_ADD}, new_node, new_node_2);
        else 
            new_node = NodeCtor(OPERATOR, (type_t){.operators = OPERATOR_SUB}, new_node, new_node_2);

        ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token);
    }

    DUMP_CURRENT_SITUATION(new_node);

    return new_node;
}

Tree_node* LangGetTerm(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* cur_token = NULL;
    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token);

    Tree_node* new_node = LangGetPow(language, number_token, status);

    DUMP_CURRENT_SITUATION(new_node);

    while (cur_token->type == OPERATOR && (cur_token->value.operators == OPERATOR_DIV || cur_token->value.operators == OPERATOR_MUL)) {
        (*number_token)++;

        Tree_node* new_node_2 = LangGetPow(language, number_token, status);
        DUMP_CURRENT_SITUATION(new_node_2);

        if (cur_token->value.operators == OPERATOR_MUL)
            new_node = NodeCtor(OPERATOR, (type_t){.operators = OPERATOR_MUL}, new_node, new_node_2);
        else 
            new_node = NodeCtor(OPERATOR, (type_t){.operators = OPERATOR_DIV}, new_node, new_node_2);

        ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token);
    }

    DUMP_CURRENT_SITUATION(new_node);

    return new_node;
}

Tree_node* LangGetPow(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* cur_token = NULL;
    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token);

    Tree_node* new_node = LangGetPrimaryExpression(language, number_token, status);

    DUMP_CURRENT_SITUATION(new_node);

    while (cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_POW) {
        (*number_token)++;

        Tree_node* new_node_2 = LangGetPrimaryExpression(language, number_token, status);
        DUMP_CURRENT_SITUATION(new_node_2);
        new_node = NodeCtor(OPERATOR, (type_t){.operators = OPERATOR_POW}, new_node, new_node_2);

        ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token);
    }

    DUMP_CURRENT_SITUATION(new_node);

    return new_node;
}

Tree_node* LangGetPrimaryExpression(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);
    
    Tree_node* cur_token = NULL;
    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token);

    Tree_node* res_node = NULL;

    if (cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_OPEN_BRACKET) {
        (*number_token)++;
        res_node = LangGetExpression(language, number_token, status);

        DUMP_CURRENT_SITUATION(res_node);

        ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token);
        if (cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_OPEN_BRACKET)
            (*number_token)++;
        else
            *status = NOT_END_SKOBKA;
    }

    else {
        res_node = LangGetNumber(language, number_token, status);
        if (res_node != NULL)
            return res_node;

        res_node = LangGetVariable(language, number_token, status);
        if (res_node != NULL)
            return res_node;

        res_node = LangGetFunction(language, number_token, status);
        if (res_node != NULL)
            return res_node;
    }

    DUMP_CURRENT_SITUATION(res_node);

    return res_node;
}

Tree_node* LangGetNumber(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* tree_node = NULL;
    ArrayGetElement(&(language->array_with_tokens), &tree_node, *number_token);

    if (tree_node->type == NUMBER) {
        (*number_token)++;
        return tree_node;
    }

    return NULL;
}

Tree_node* LangGetVariable(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* tree_node = NULL;
    ArrayGetElement(&(language->array_with_tokens), &tree_node, *number_token);

    if (tree_node->type == VARIABLE) {
        (*number_token)++;
        return tree_node;
    }

    return NULL;
}

Tree_node* LangGetFunction(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* tree_node = NULL;
    ArrayGetElement(&(language->array_with_tokens), &tree_node, *number_token);

    if (tree_node->type == OPERATOR) {

        for (size_t i = 0; i < sizeof(key_words) / sizeof(key_words[0]); ++i) {
            if (tree_node->value.operators == key_words[i].type && 
                (key_words[i].type != OPERATOR_IF && key_words[i].type != OPERATOR_WHILE)) {
                    (*number_token)++;
                    return tree_node;
                }
        }
    }

    return NULL;
}