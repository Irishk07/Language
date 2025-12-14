#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "array.h"
#include "../common.h"
#include "front_end.h"
#include "onegin.h"
#include "syntax_analize.h"
#include "tree.h"

// узел список аргументов - он как узел с точкой запятой

// +++++++++++++++++|||||+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Comandir          ::= Operators+ '$'
// Operators         ::= { Assignment | WhileOrIf | '{'Operators+'}' | Print} ';'                                              
// WhileOrIf         ::= ["if", "while"]'('Expression')'Operators Else?
// Else              ::= "else" Operators
// Print             ::= "print" '('Expression')'
// Assignment        ::= Variable'='Expression
// Expression        ::= Term{[+ -]Term}*
// Term              ::= Pow{[* /]Pow}*
// Pow               ::= PrimaryExpression{[^]PrimaryExpression}*
// PrimaryExpression ::= '('Expression')' | Number | Variable | MathFunction | Input                                    
// Number            ::= [0-9]+
// Variable          ::= [a-zA-Z_][a-zA-z0-9_]*
// MathFunction      ::= [ln, sin, cos, tg, ctg, arcsin, arccos, arctg, arcctg, sh, ch, th, cth]'('Expression')'
// Input             ::= [input]
// +++++++++++++++++|||||+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


Tree_node* LangGetComandir(Language* language, Tree_status* status) {
    assert(language);
    assert(status);

    size_t number_token = 0;

    Tree_node* result_node = NULL;

    do {
        Tree_node* operator_node = LangGetOperators(language, &number_token, status);

        if (operator_node == NULL)
            break;

        if (result_node == NULL)
            result_node = NodeCtor(OPERATOR, (type_t){.operators = OPERATOR_COMMON}, operator_node, NULL);
        else if (result_node->right_node == NULL)
            result_node->right_node = operator_node;
        else
            result_node = NodeCtor(OPERATOR, (type_t){.operators = OPERATOR_COMMON}, result_node, operator_node);

        TreeHTMLDump(language, result_node, DUMP_INFO, NOT_ERROR_DUMP);
    }
    while (number_token < language->array_with_tokens.size);

    Tree_node* cur_token = NULL;
    ArrayGetElement(&(language->array_with_tokens), &cur_token, number_token);
    if ((cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_FINISH_SYMBOL) == 0) {
        *status = NOT_END_SYMBOL;
        return NULL;
    }

    free(cur_token);
    number_token++;

    return result_node;
}

Tree_node* LangGetOperators(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* operator_node = LangGetPrint(language, number_token, status);
    if (operator_node != NULL)
        return operator_node;
    DUMP_CURRENT_SITUATION(operator_node);

    operator_node = LangGetWhileOrIf(language, number_token, status);
    if (operator_node != NULL)
        return operator_node;
    DUMP_CURRENT_SITUATION(operator_node);

    operator_node = LangGetAssignment(language, number_token, status);
    if (operator_node != NULL)
        return operator_node;
    DUMP_CURRENT_SITUATION(operator_node);

    Tree_node* cur_token = NULL;
    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); // wait {
    if (cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_OPEN_FIGURE) {
        free(cur_token);
        (*number_token)++;

        ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); // wait }
        while((cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_CLOSE_FIGURE) == 0) {
            Tree_node* operator_node_2 = LangGetOperators(language, number_token, status);

            DUMP_CURRENT_SITUATION(operator_node_2);

            if (operator_node == NULL)
                operator_node = operator_node_2;
            else if (operator_node != NULL && operator_node->right_node == NULL)
                operator_node->right_node = operator_node_2;
            else
                operator_node = NodeCtor(OPERATOR, (type_t){.operators = OPERATOR_COMMON}, operator_node_2, operator_node);

            DUMP_CURRENT_SITUATION(operator_node);

            ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); // wait }
        } 
        
        free(cur_token);
        (*number_token)++;
    }

    DUMP_CURRENT_SITUATION(operator_node);

    return operator_node;
}

Tree_node* LangGetWhileOrIf(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* cur_token = NULL;
    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); // wait while or if

    if ((cur_token->type == OPERATOR && 
        (cur_token->value.operators == OPERATOR_WHILE || cur_token->value.operators == OPERATOR_IF)) == 0)
        return NULL;

    (*number_token)++;
    Tree_node* while_if_operator = cur_token; // wait while or if
    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); // wait (
    if (cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_OPEN_BRACKET) {
        free(cur_token);
        (*number_token)++;

        Tree_node* expression_node = LangGetExpression(language, number_token, status);

        ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); // wait )
        if (cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_CLOSE_BRACKET) {
            free(cur_token);
            (*number_token)++;

            Tree_node* operator_node = LangGetOperators(language, number_token, status);

            while_if_operator->left_node = expression_node;
            while_if_operator->right_node = operator_node;
        }
    }

    if (while_if_operator->value.operators == OPERATOR_IF)
        return LangGetElse(language, number_token, status, while_if_operator);

    return while_if_operator;
}

Tree_node* LangGetElse(Language* language, size_t* number_token, Tree_status* status, Tree_node* if_operator_node) {
    assert(language);
    assert(status);

    Tree_node* cur_token = NULL;
    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); // wait else

    if (cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_ELSE) {
        (*number_token)++;

        Tree_node* operator_node = LangGetOperators(language, number_token, status);

        cur_token->left_node  = if_operator_node->right_node;  // value if
        cur_token->right_node = operator_node; // value else
        if_operator_node->right_node = cur_token;
    }

    return if_operator_node;
}

Tree_node* LangGetAssignment(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* variable_node = LangGetVariable(language, number_token, status);
    if (variable_node == NULL)
        return NULL;

    DUMP_CURRENT_SITUATION(variable_node);

    Tree_node* cur_token = NULL;
    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); // wait = or :=

    if (cur_token->type == OPERATOR && 
        (cur_token->value.operators == OPERATOR_EQUAL || cur_token->value.operators == OPERATOR_CHANGE)) {
        (*number_token)++;

        Tree_node* expression_node = LangGetExpression(language, number_token, status);
        Tree_node* equal_node      = cur_token;

        DUMP_CURRENT_SITUATION(expression_node);

        ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); // wait ;

        if ((cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_COMMON) == 0)
            return NULL;

        (*number_token)++;

        equal_node->left_node  = variable_node;
        equal_node->right_node = expression_node;

        cur_token->left_node   = equal_node; // left_node of node with ;

        DUMP_CURRENT_SITUATION(variable_node);

        return cur_token;
    }

    return NULL;
}

Tree_node* LangGetExpression(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* term_node = LangGetTerm(language, number_token, status);

    DUMP_CURRENT_SITUATION(term_node);

    Tree_node* cur_token = NULL;
    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); // wait + or -

    while (cur_token->type == OPERATOR && (cur_token->value.operators == OPERATOR_ADD || cur_token->value.operators == OPERATOR_SUB)) {
        (*number_token)++;

        Tree_node* term_node_2 = LangGetTerm(language, number_token, status);

        cur_token->left_node  = term_node;
        cur_token->right_node = term_node_2;

        DUMP_CURRENT_SITUATION(term_node_2);

        term_node = cur_token;

        ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); // wait + or -
    }

    DUMP_CURRENT_SITUATION(cur_token);

    return term_node;
}

Tree_node* LangGetTerm(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* pow_node = LangGetPow(language, number_token, status);

    DUMP_CURRENT_SITUATION(pow_node);

    Tree_node* cur_token = NULL;
    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); // wait * or /

    while (cur_token->type == OPERATOR && (cur_token->value.operators == OPERATOR_DIV || cur_token->value.operators == OPERATOR_MUL)) {
        (*number_token)++;

        Tree_node* pow_node_2 = LangGetPow(language, number_token, status);

        cur_token->left_node = pow_node;
        cur_token->right_node = pow_node_2;

        DUMP_CURRENT_SITUATION(pow_node_2);

        pow_node = cur_token;

        ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); // wait * or /
    }

    DUMP_CURRENT_SITUATION(pow_node);

    return pow_node;
}

Tree_node* LangGetPow(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* p_e_node = LangGetPrimaryExpression(language, number_token, status);

    DUMP_CURRENT_SITUATION(p_e_node);

    Tree_node* cur_token = NULL;
    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); // wait ^

    while (cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_POW) {
        (*number_token)++;

        Tree_node* p_e_node_2 = LangGetPrimaryExpression(language, number_token, status);
        
        cur_token->left_node = p_e_node;
        cur_token->right_node = p_e_node_2;

        DUMP_CURRENT_SITUATION(p_e_node_2);

        p_e_node = cur_token;

        ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); // wait ^
    }

    DUMP_CURRENT_SITUATION(p_e_node);

    return p_e_node;
}

Tree_node* LangGetPrimaryExpression(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* element_node = NULL;

    Tree_node* cur_token = NULL;
    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); // wait (

    if (cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_OPEN_BRACKET) { 
        free(cur_token);
        (*number_token)++;

        element_node = LangGetExpression(language, number_token, status);

        DUMP_CURRENT_SITUATION(element_node);

        ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); // wait )
        if (cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_CLOSE_BRACKET) {
            free(cur_token);
            (*number_token)++;
        }    
        else
            *status = NOT_END_SKOBKA;
    }

    else {
        element_node = LangGetNumber(language, number_token, status);
        if (element_node != NULL)
            return element_node;

        element_node = LangGetVariable(language, number_token, status);
        if (element_node != NULL)
            return element_node;

        element_node = LangGetMathFunction(language, number_token, status);
        if (element_node != NULL)
            return element_node;

        element_node = LangGetInput(language, number_token, status);
        if (element_node != NULL)
            return element_node;
    }

    DUMP_CURRENT_SITUATION(element_node);

    return element_node;
}

Tree_node* LangGetNumber(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* number_node = NULL;
    ArrayGetElement(&(language->array_with_tokens), &number_node, *number_token);

    if (number_node->type == NUMBER) {
        (*number_token)++;
        return number_node;
    }

    return NULL;
}

Tree_node* LangGetVariable(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* variable_node = NULL;
    ArrayGetElement(&(language->array_with_tokens), &variable_node, *number_token);

    if (variable_node->type == VARIABLE) {
        (*number_token)++;
        return variable_node;
    }

    return NULL;
}

Tree_node* LangGetMathFunction(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* math_func_node = NULL;
    ArrayGetElement(&(language->array_with_tokens), &math_func_node, *number_token);

    if (math_func_node->type == OPERATOR) {
        for (size_t i = 0; i < sizeof(key_words) / sizeof(key_words[0]); ++i) {
            if (math_func_node->value.operators == key_words[i].type && 
                (key_words[i].type != OPERATOR_IF && key_words[i].type != OPERATOR_WHILE 
                && key_words[i].type != OPERATOR_INPUT && key_words[i].type != OPERATOR_PRINT)) {
                    (*number_token)++;
                    return math_func_node;
            }
        }
    }

    return NULL;
}

Tree_node* LangGetInput(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* scanf_node = NULL;
    ArrayGetElement(&(language->array_with_tokens), &scanf_node, *number_token);

    if (scanf_node->type == OPERATOR && scanf_node->value.operators == OPERATOR_INPUT) { // wait input
        (*number_token)++;

        Tree_node* argument_node = NULL;
        ArrayGetElement(&(language->array_with_tokens), &argument_node, *number_token);

        if (argument_node->type == OPERATOR && argument_node->value.operators == OPERATOR_OPEN_BRACKET) { // wait (
            (*number_token)++;
            free(argument_node);
            ArrayGetElement(&(language->array_with_tokens), &argument_node, *number_token);
       
            if (argument_node->type == OPERATOR && argument_node->value.operators == OPERATOR_CLOSE_BRACKET) { // wait )
                (*number_token)++;
                free(argument_node);

                return scanf_node;
            }
        }
    }

    return NULL;
}

Tree_node* LangGetPrint(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* print_node = NULL;
    ArrayGetElement(&(language->array_with_tokens), &print_node, *number_token);

    if (print_node->type == OPERATOR && print_node->value.operators == OPERATOR_PRINT) { // wait print
        (*number_token)++;
        
        Tree_node* argument_node = NULL;
        ArrayGetElement(&(language->array_with_tokens), &argument_node, *number_token);

        if (argument_node->type == OPERATOR && argument_node->value.operators == OPERATOR_OPEN_BRACKET) { // wait (
            (*number_token)++;
            free(argument_node);
            
            Tree_node* expression_node = LangGetExpression(language, number_token, status);

            print_node->left_node = expression_node;

            ArrayGetElement(&(language->array_with_tokens), &argument_node, *number_token);

            if (argument_node->type == OPERATOR && argument_node->value.operators == OPERATOR_CLOSE_BRACKET) { // wait )
                (*number_token)++;
                free(argument_node);

                ArrayGetElement(&(language->array_with_tokens), &argument_node, *number_token);
                if ((argument_node->type == OPERATOR && argument_node->value.operators == OPERATOR_COMMON) == 0)
                    return NULL;

                (*number_token)++;
                argument_node->left_node = print_node;
                return argument_node;
            }
        }
    }

    return NULL;
}