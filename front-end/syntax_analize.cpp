#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "../array.h"
#include "../common.h"
#include "front_end.h"
#include "onegin.h"
#include "syntax_analize.h"
#include "../tree.h"


// +|||++++++++++++ //
// *   0+ times     //
// +   1+ times     //
// ?   0 or 1 times //
// +|||++++++++++++ //


// +++++++++++++++++|||||++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //
// Comandir           ::= {MainFunction | Operators+ | DefinitionFunction+} 'Bye'                                                                       //
// MainFunction       ::= "Welcome!" Variable '+___-' {Return | Operators+} '-___+'                                                                     // 
// DefinitionFunction ::= "Recipe:" Variable '*_^' Variable{'and' Variable}* '^_*' '+___-' {Return | Operators+} '-___+'                                // 
// Return             ::= "Return the money for" Expression                                                                                             //
// Operators          ::= {CallFunction | AssignmentOrChange | Changes | WhileOrIf | '+___-' Operators+ '-___+' | Print | Return} ';)'                                         //     
// WhileOrIf          ::= {If | While}                                                                                                                  //
// While              ::= "I didn't write complaint only because" '*_^' Expression  {[Compares | Equals] Expression}? '^_*' Operators                   //
// If                 ::= "We resolve conflict if you call chef and check" '*_^' Expression  {[Compares | Equals] Expression}? '^_*' Operators Else?    //
// Else               ::= "We won't resolve conflict. But" Operators                                                                                    //
// Compares           ::= [< >] Expression                                                                                                              //
// Equals             ::= "==" Expression                                                                                                               //
// Print              ::= "Calculate how much dish" '*_^' Expression '^_*'                                                                              //
// AssignmentOrChange ::= {Assignment | Changes}                                                                                                        //
// Changes            ::= "I want to change my order:" Expression '->' Variable                                                                         //
//                        "I repeat changes:" Expression '->' Variable                                                                                  //
// Assignment         ::= "I want to place an order: bring to me" Expression '->' Variable                                                              //
//                        "I repeat my order:" Expression '->' Variable                                                                                 //
// Expression         ::= Term{[+ -]Term}*                                                                                                              //
// Term               ::= Pow{[* /]Pow}*                                                                                                                //
// Pow                ::= PrimaryExpression{[^]PrimaryExpression}*                                                                                      //
// PrimaryExpression  ::= '*_^' Expression '^_*' | Number | Variable | MathFunction | Input | CallFunction                                              //
// Number             ::= '-'? [0-9]+                                                                                                                        //
// Variable           ::= '-'? [a-zA-Z_][a-zA-z0-9_]*                                                                                                   //
// MathFunction       ::= [ln, sin, cos, tg, ctg, arcsin, arccos, arctg, arcctg, sh, ch, th, cth] '*_^' Expression '^_*'                                //
// Input              ::= "Bring menu"                                                                                                                  //
// CallFunction       ::= "Waiter!" Variable '*_^' Variable{'and' Variable}* '^_*'                                                                      //
// +++++++++++++++++|||||++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //



Tree_node* LangGetComandir(Language* language, Tree_status* status) {
    assert(language);
    assert(status);

    size_t number_token = 0;

    Tree_node* result_node = NULL;

    do {
        Tree_node* cur_node = LangGetMainFunction(language, &number_token, status);
    
        if (cur_node == NULL)
            cur_node = LangGetDefinitionFunction(language, &number_token, status);
        if (cur_node == NULL)
            cur_node = LangGetOperators(language, &number_token, status);
        if (cur_node == NULL)
            break;

        if (result_node == NULL)
            result_node = NodeCtor(OPERATOR, (type_t){.operators = OPERATOR_COMMON}, cur_node, NULL);
        else if (result_node->right_node == NULL)
            result_node->right_node = cur_node;
        else
            result_node = NodeCtor(OPERATOR, (type_t){.operators = OPERATOR_COMMON}, result_node, cur_node);

        TreeHTMLDump(&language->dump_info, &language->array_with_variables, result_node, DUMP_INFO, NOT_ERROR_DUMP);
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

Tree_node* LangGetMainFunction(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* cur_token = NULL;
    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); 
    if (cur_token->type != OPERATOR || cur_token->value.operators != OPERATOR_MAIN_FUNCTION)
        return NULL;

    (*number_token)++;
    Tree_node* func_node = cur_token;

    Tree_node* name_node = LangGetVariable(language, number_token, status);
    if (name_node == NULL)
        return NULL;

    Tree_node* body_node = LangGetFunctionBody(language, number_token, status);
    if (body_node == NULL)
        return NULL;

    func_node->left_node  = name_node;
    func_node->right_node = body_node;

    return func_node;
}

Tree_node* LangGetDefinitionFunction(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* cur_token = NULL;
    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); 
    if (cur_token->type != OPERATOR || cur_token->value.operators != OPERATOR_DEF_FUNCTION)
        return NULL;

    (*number_token)++;
    Tree_node* func_node = cur_token;

    Tree_node* name_node = LangGetVariable(language, number_token, status);
    if (name_node == NULL)
        return NULL;

    Tree_node* param_node = LangGetParams(language, number_token, status);
    if (param_node == NULL)
        return NULL;
    param_node = NodeCtor(OPERATOR, (type_t){.operators = OPERATOR_PARAM}, name_node, param_node);
    func_node->left_node = param_node;

    Tree_node* body_node = LangGetFunctionBody(language, number_token, status);
    if (body_node == NULL)
        return NULL;
    func_node->right_node = body_node;

    return func_node;
}

Tree_node* LangGetParams(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* cur_token = NULL;

    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); 
    if (cur_token->type != OPERATOR || cur_token->value.operators != OPERATOR_OPEN_BRACKET) // wait (
        return NULL;
    (*number_token)++;
    free(cur_token);

    Tree_node* variable_node = LangGetVariable(language, number_token, status);
    if (variable_node == NULL)
        return NULL;

    Tree_node* param_node = NodeCtor(OPERATOR, (type_t){.operators = OPERATOR_PARAM}, variable_node, NULL);

    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token);
    while (cur_token->type != OPERATOR || cur_token->value.operators != OPERATOR_CLOSE_BRACKET) { // wait )
        if (cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_PARAM) { // wait ,
            (*number_token)++;

            variable_node = LangGetVariable(language, number_token, status);
            if (variable_node == NULL)
                return NULL;

            if (param_node->right_node == NULL) {
                free(cur_token);
                param_node->right_node = variable_node;
            }    
            else {
                cur_token->left_node   = param_node->right_node;
                cur_token->right_node  = variable_node;
                param_node->right_node = cur_token;
            }

            ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token);
        }
    }

    (*number_token)++;
    free(cur_token); // here )

    return param_node;
}

Tree_node* LangGetFunctionBody(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* cur_token = NULL;
    Tree_node* body_node = NULL;

    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token);
    if (cur_token->type != OPERATOR || cur_token->value.operators != OPERATOR_OPEN_FIGURE) // wait {
        return NULL;
    (*number_token)++;
    free(cur_token);

    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token);
    while (cur_token->type != OPERATOR || cur_token->value.operators != OPERATOR_CLOSE_FIGURE) { // wait }
        Tree_node* operators_node = LangGetOperators(language, number_token, status);
        if (operators_node == NULL)
            operators_node = LangGetReturn(language, number_token, status);

        if (operators_node == NULL)
            return NULL;

        if (body_node == NULL)
            body_node = operators_node;
        else
            body_node = NodeCtor(OPERATOR, (type_t){.operators = OPERATOR_COMMON}, body_node, operators_node);

        ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token);
    }

    if (body_node != NULL) {
        (*number_token)++;
        free(cur_token);

        return body_node;
    }

    return NULL;
}

Tree_node* LangGetReturn(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* return_node = NULL;
    ArrayGetElement(&(language->array_with_tokens), &return_node, *number_token);
    if (return_node->type != OPERATOR || return_node->value.operators != OPERATOR_RETURN)
        return NULL;
    (*number_token)++;

    Tree_node* expression_node = LangGetExpression(language, number_token, status);

    if (expression_node == NULL)
        return NULL;
    return_node->left_node = expression_node;

    Tree_node* cur_token = NULL;
    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token);
    if (cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_COMMON) {
        (*number_token)++;
        cur_token->right_node = return_node;
    }     

    return cur_token;
}

Tree_node* LangGetOperators(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* operator_node = LangGetCallFuntion(language, number_token, status);
    if (operator_node != NULL)
        return operator_node;
    DUMP_CURRENT_SITUATION(operator_node);

    operator_node = LangGetPrint(language, number_token, status);
    if (operator_node != NULL)
        return operator_node;
    DUMP_CURRENT_SITUATION(operator_node);

    operator_node = LangGetReturn(language, number_token, status);
    if (operator_node != NULL)
        return operator_node;
    DUMP_CURRENT_SITUATION(operator_node);

    operator_node = LangGetWhileOrIf(language, number_token, status);
    if (operator_node != NULL)
        return operator_node;
    DUMP_CURRENT_SITUATION(operator_node);

    operator_node = CheckEqualAssignmentOrChange(language, number_token, status);
    if (operator_node != NULL)
        return operator_node;
    DUMP_CURRENT_SITUATION(operator_node);

    Tree_node* cur_token = NULL;
    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); // wait +___-
    if (cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_OPEN_FIGURE) {
        free(cur_token);
        (*number_token)++;

        ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); // wait -___+
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

            ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); // wait -___+
        } 
        
        free(cur_token);
        (*number_token)++;
    }

    DUMP_CURRENT_SITUATION(operator_node);

    return operator_node;
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

        if (argument_node->type == OPERATOR && argument_node->value.operators == OPERATOR_OPEN_BRACKET) { // wait *_^
            (*number_token)++;
            free(argument_node);
            
            Tree_node* expression_node = LangGetExpression(language, number_token, status);

            print_node->left_node = expression_node;

            ArrayGetElement(&(language->array_with_tokens), &argument_node, *number_token);

            if (argument_node->type == OPERATOR && argument_node->value.operators == OPERATOR_CLOSE_BRACKET) { // wait ^_*
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
    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); // wait *_^
    if (cur_token->type == OPERATOR && cur_token->value.operators == OPERATOR_OPEN_BRACKET) {
        free(cur_token);
        (*number_token)++;

        Tree_node* expression_node = LangGetExpression(language, number_token, status);

        Tree_node* sign_node = LangGetCompares(language, number_token, status);
        if (sign_node == NULL)
            sign_node = LangGetEquals(language, number_token, status);

        if (sign_node != NULL) {
            sign_node->left_node = expression_node;
            expression_node = sign_node;
        }

        ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); // wait ^_*
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

        cur_token->left_node         = if_operator_node->right_node;  // value if
        cur_token->right_node        = operator_node; // value else
        if_operator_node->right_node = cur_token;
    }

    return if_operator_node;
}

Tree_node* LangGetCompares(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* sign_node = NULL;
    ArrayGetElement(&(language->array_with_tokens), &sign_node, *number_token);

    if (sign_node->type == OPERATOR && (sign_node->value.operators == OPERATOR_ABOVE || sign_node->value.operators == OPERATOR_BEFORE)) { // wait < or >
        (*number_token)++;

        Tree_node* expression_node = NULL;
        expression_node = LangGetExpression(language, number_token, status);

        sign_node->right_node = expression_node;

        return sign_node; 
    }

    return NULL;
}

Tree_node* LangGetEquals(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* sign_node = NULL;
    ArrayGetElement(&(language->array_with_tokens), &sign_node, *number_token);

    if (sign_node->type == OPERATOR && sign_node->value.operators == OPERATOR_EQUAL) { // wait ==
        (*number_token)++;

        Tree_node* expression_node = NULL;
        expression_node = LangGetExpression(language, number_token, status);

        sign_node->right_node = expression_node;

        return sign_node; 
    }

    return NULL;
}

Tree_node* CheckEqualAssignmentOrChange(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* first_node = LangGetAssignmentOrChange(language, number_token, status);
    Tree_node* second_node = LangGetAssignmentOrChange(language, number_token, status);

    DUMP_CURRENT_SITUATION(first_node);
    DUMP_CURRENT_SITUATION(second_node);

    if (CompareTrees(first_node, second_node) != EQUAL) {
        LanguageNodeDtor(language, first_node);
        LanguageNodeDtor(language, second_node);
        return NULL;
    }

    LanguageNodeDtor(language, second_node);

    return first_node;
}

Status_of_comparing CompareTrees(Tree_node* tree_node_1, Tree_node* tree_node_2) {
    if (tree_node_1 == NULL && tree_node_2 == NULL)
        return EQUAL;
    
    if (tree_node_1 == NULL || tree_node_2 == NULL)
        return DIFFERENT;

    if (tree_node_1->type != tree_node_2->type)
        return DIFFERENT;

    Type_node type_node = tree_node_1->type;

    if (type_node == NUMBER) {
        if (tree_node_1->value.number != tree_node_2->value.number)
            return DIFFERENT;
    }
    else if (type_node == VARIABLE) {
        if (strcmp(NameOfVariable(tree_node_1), NameOfVariable(tree_node_2)) != 0)
            return DIFFERENT;
    } 
    else if (type_node == OPERATOR) {
        if (tree_node_1->value.operators != tree_node_2->value.operators)
            return DIFFERENT;
    } 

    Status_of_comparing status_compare_left_parents  = CompareTrees(tree_node_1->left_node, tree_node_2->left_node);
    Status_of_comparing status_compare_right_parents = CompareTrees(tree_node_1->right_node, tree_node_2->right_node);

    if (status_compare_left_parents == DIFFERENT || status_compare_right_parents == DIFFERENT)
        return DIFFERENT;
    
    return EQUAL;
}

Tree_node* LangGetAssignmentOrChange(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* cur_token = NULL;
    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); // wait = or :=

    if (cur_token->type == OPERATOR && 
        (cur_token->value.operators == OPERATOR_ASSIGNMENT || cur_token->value.operators == OPERATOR_CHANGE)) {
        (*number_token)++;

        Tree_node* expression_node = LangGetExpression(language, number_token, status);
        Tree_node* equal_node      = cur_token;

        DUMP_CURRENT_SITUATION(expression_node);

        ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token);
        if (cur_token->type != OPERATOR || cur_token->value.operators != OPERATOR_MATCH)
            return NULL;
        (*number_token)++;
        free(cur_token);

        Tree_node* variable_node = LangGetVariable(language, number_token, status);
        if (variable_node == NULL)
            return NULL;

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

        element_node = LangGetInput(language, number_token, status);
        if (element_node != NULL)
            return element_node;

        element_node = LangGetCallFuntion(language, number_token, status);
        if (element_node != NULL)
            return element_node;

        element_node = LangGetMathFunction(language, number_token, status);
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

    if (number_node->type == OPERATOR && number_node->value.operators == OPERATOR_SUB) {
        (*number_token)++;
        free(number_node);

        ArrayGetElement(&(language->array_with_tokens), &number_node, *number_token);

        if (number_node->type == NUMBER) {
            (*number_token)++;
            number_node->value.number *= -1;
            return number_node;
        }
    }

    else if (number_node->type == NUMBER) {
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

    if (variable_node->type == OPERATOR && variable_node->value.operators == OPERATOR_SUB) {
        (*number_token)++;
        free(variable_node);

        ArrayGetElement(&(language->array_with_tokens), &variable_node, *number_token);

        if (variable_node->type == VARIABLE) {
            (*number_token)++;
            return NodeCtor(OPERATOR, (type_t){.operators = OPERATOR_MUL}, variable_node, NodeCtor(NUMBER, (type_t){.number = -1}, NULL, NULL));
        }
    }

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

    if (math_func_node->type != OPERATOR) 
        return NULL;

    for (size_t i = 0; i < sizeof(key_words) / sizeof(key_words[0]); ++i) {
        if (math_func_node->value.operators == key_words[i].type && 
            (key_words[i].type != OPERATOR_IF && key_words[i].type != OPERATOR_WHILE && key_words[i].type != OPERATOR_PRINT)) {
                (*number_token)++;

                Tree_node* cur_token = NULL;
                ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token);
                if (cur_token->type != OPERATOR || cur_token->value.operators != OPERATOR_OPEN_BRACKET) // wait (
                    return NULL;

                (*number_token)++;
                free(cur_token);

                Tree_node* expression_node = LangGetExpression(language, number_token, status);
                if (expression_node == NULL)
                    return NULL;

                ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token);
                if (cur_token->type != OPERATOR || cur_token->value.operators != OPERATOR_CLOSE_BRACKET) // wait )
                    return NULL;

                (*number_token)++;
                free(cur_token);
                
                math_func_node->left_node = expression_node;

                return math_func_node;
        }
    } 

    return NULL;
}

Tree_node* LangGetInput(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* input_node = NULL;
    ArrayGetElement(&(language->array_with_tokens), &input_node, *number_token);

    if (input_node->type == OPERATOR && input_node->value.operators == OPERATOR_INPUT) { // wait input
        (*number_token)++;

        return input_node;
    }

    return NULL;
}

Tree_node* LangGetCallFuntion(Language* language, size_t* number_token, Tree_status* status) {
    assert(language);
    assert(status);

    Tree_node* cur_token = NULL;
    ArrayGetElement(&(language->array_with_tokens), &cur_token, *number_token); 
    if (cur_token->type != OPERATOR || cur_token->value.operators != OPERATOR_CALL_FUNCTION)
        return NULL;

    (*number_token)++;
    Tree_node* call_node = cur_token;

    Tree_node* name_node = LangGetVariable(language, number_token, status);
    if (name_node == NULL)
        return NULL;

    Tree_node* param_node = LangGetParams(language, number_token, status);
    if (param_node == NULL)
        return NULL;

    call_node->left_node = name_node;
    call_node->right_node = param_node;

    return call_node;
}