#include <assert.h>
#include <math.h>

#include "differentiator.h"

#include "../common.h"
#include "middle_end.h"
#include "tree.h"


#define NUMBER_NODE_CTOR(num) NodeCtor(NUMBER, (type_t){.number = num}, NULL, NULL)

void OptimizationTree(Language* language, Tree_node** old_node) {
    assert(language);

    if (old_node == NULL) 
        return;

    if ((*old_node)->left_node)
        OptimizationTree(language, &(*old_node)->left_node);
    if ((*old_node)->right_node)
        OptimizationTree(language, &(*old_node)->right_node);

    if ((*old_node)->type == OPERATOR)
        OptimizationNode(language, old_node);
}

void OptimizationNode(Language* language, Tree_node** old_node) {
    assert(language);
    assert(old_node);

    Tree tree = language->tree;

    if (IsConstantNode(language, *old_node)) {
        int result = (int)Calculating(language, (*old_node));
        LanguageNodeDtor(language, *old_node);
        *old_node = NUMBER_NODE_CTOR(result);
        return;
    }

    OptimizationOneNode(language, old_node, (*old_node)->left_node, (*old_node)->right_node);
    OptimizationOneNode(language, old_node, (*old_node)->right_node, (*old_node)->left_node);

    OptimizationZeroNode(language, &tree, old_node, (*old_node)->left_node, (*old_node)->right_node);
    OptimizationZeroNode(language, &tree, old_node, (*old_node)->right_node, (*old_node)->left_node);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

void OptimizationZeroNode(Language* language, Tree* tree, Tree_node** tree_node, Tree_node* first_child, Tree_node* second_child) {
    assert(language);
    assert(tree);
    assert(tree_node);

    if (IsZeroNode(first_child)) {
        switch ((*tree_node)->value.operators) {
            case OPERATOR_ADD:
            case OPERATOR_SUB:
                {
                    Tree_node* child = second_child;
                    LanguageNodeDtor(language, first_child);
                    free(*tree_node);
                    (*tree_node) = child;
                    return;
                }
            case OPERATOR_MUL:
                {
                    LanguageNodeDtor(language, (*tree_node));
                    (*tree_node) = NUMBER_NODE_CTOR(0.0);
                    return;
                }
            case OPERATOR_POW:
                {
                    LanguageNodeDtor(language, (*tree_node));
                    (*tree_node) = NUMBER_NODE_CTOR(1.0);
                    return;
                }
            default:
                break;
        }
    }
}

void OptimizationOneNode(Language* language, Tree_node** tree_node, Tree_node* first_child, Tree_node* second_child) {
    assert(language);
    assert(tree_node);

    if (IsOneNode(first_child)) {
        switch ((*tree_node)->value.operators) {
            case OPERATOR_MUL:
            case OPERATOR_DIV:
            case OPERATOR_POW:
                {
                    Tree_node* child = second_child;
                    LanguageNodeDtor(language, first_child);
                    free((*tree_node));
                    (*tree_node) = child;
                    return;
                }            
            default:
                break;
        }
    }
}

#pragma GCC diagnostic pop

bool IsConstantNode(Language* language, Tree_node* node) {
    assert(language);

    if (node == NULL) return true;
    
    switch (node->type) {
        case NUMBER:
            return true;
        case VARIABLE:
            return false;
        case OPERATOR:
            return IsConstantNode(language, node->left_node) && 
                   IsConstantNode(language, node->right_node);
        case WRONG_TYPE:
        default:
            return false;
    }
}

bool IsZeroNode(Tree_node* node) {
    if (node == NULL) 
        return false;

    return (node->type == NUMBER && fabs(node->value.number) < EPS);
}

bool IsOneNode(Tree_node* node) {
    if (node == NULL) 
        return false;

    return (node->type == NUMBER && fabs(node->value.number - 1.0) < EPS);
}


#define CALCULATING_OPERATORS(sign)                                                                           \
        return Calculating(language, tree_node->left_node) sign Calculating(language, tree_node->right_node);

#define CALCULATING_DIV                                               \
{                                                                     \
    double second_num = Calculating(language, tree_node->right_node); \
    if (fabs(second_num) < EPS)                                       \
        return NAN;                                                   \
    return Calculating(language, tree_node->left_node) / second_num;  \
}

#define CALCULATING_CTG                                             \
{                                                                   \
    double ans = Calculating(language, tree_node->left_node);       \
    double tg = tan(ans);                                           \
    if (fabs(tg) < EPS)                                             \
        return NAN;                                                 \
    return 1.0 / tg;                                                \
}

#define CALCULATING_LN                                              \
{                                                                   \
    double ans = Calculating(language, tree_node->left_node);       \
    if (ans <= 0)                                                   \
        return NAN;                                                 \
    return log(ans);                                                \
}

#define CALCULATING_LOG                                                     \
{                                                                           \
    double base = Calculating(language, tree_node->left_node);              \
    double argument = Calculating(language, tree_node->right_node);         \
                                                                            \
    if (base <= 0 || fabs(base - 1) < EPS)                                  \
        return NAN;                                                         \
    if (argument <= 0)                                                      \
        return NAN;                                                         \
                                                                            \
    return log(argument) / log(base);                                       \
}

#define CALCULATING_ARCSIN                                          \
{                                                                   \
    double ans = Calculating(language, tree_node->left_node);       \
    if (ans < -1.0 || ans > 1.0)                                    \
        return NAN;                                                 \
    return asin(ans);                                               \
}

#define CALCULATING_ARCCOS                                          \
{                                                                   \
    double ans = Calculating(language, tree_node->left_node);       \
    if (ans < -1.0 || ans > 1.0)                                    \
        return NAN;                                                 \
    return acos(ans);                                               \
}

#define CALCULATING_ARCCTG                                          \
{                                                                   \
    double ans = Calculating(language, tree_node->left_node);       \
    return M_PI_2 - atan(ans);                                      \
}

#define CALCULATING_CTH                                             \
{                                                                   \
    double ans = Calculating(language, tree_node->left_node);       \
    if (fabs(sinh(ans)) < EPS)                                      \
        return NAN;                                                 \
    return cosh(ans) / sinh(ans);                                   \
}

double Calculating(Language* language, Tree_node* tree_node) {
    if (tree_node == NULL)
        return NAN;

    switch (tree_node->type) {
        case NUMBER:
            return tree_node->value.number; break;
        case VARIABLE:
            return ValueOfVariable(language, tree_node); break;
        case OPERATOR:
            switch (tree_node->value.operators) {
                case OPERATOR_ADD:
                    CALCULATING_OPERATORS(ADD);
                case OPERATOR_SUB:
                    CALCULATING_OPERATORS(SUB);  
                case OPERATOR_MUL:
                    CALCULATING_OPERATORS(MUL);
                case OPERATOR_DIV:
                    CALCULATING_DIV;
                case OPERATOR_POW:
                    return pow(Calculating(language, tree_node->left_node), Calculating(language, tree_node->right_node));
                case OPERATOR_SIN:
                    return sin(Calculating(language, tree_node->left_node));
                case OPERATOR_COS:
                    return cos(Calculating(language, tree_node->left_node));
                case OPERATOR_TG:
                    return tan(Calculating(language, tree_node->left_node));
                case OPERATOR_CTG: 
                    CALCULATING_CTG;
                case OPERATOR_LN: 
                    CALCULATING_LN;
                case OPERATOR_LOG:
                    CALCULATING_LOG;
                case OPERATOR_ARCSIN: 
                    CALCULATING_ARCSIN;
                case OPERATOR_ARCCOS: 
                    CALCULATING_ARCCOS;
                case OPERATOR_ARCTG:
                    return atan(Calculating(language, tree_node->left_node));
                case OPERATOR_ARCCTG: 
                    CALCULATING_ARCCTG;
                case OPERATOR_SH:
                    return sinh(Calculating(language, tree_node->left_node));
                case OPERATOR_CH:
                    return cosh(Calculating(language, tree_node->left_node));
                case OPERATOR_TH:
                    return tanh(Calculating(language, tree_node->left_node));
                case OPERATOR_CTH: 
                    CALCULATING_CTH;
                case OPERATOR_EQUAL:
                case OPERATOR_COMMON:
                case OPERATOR_IF:
                case OPERATOR_WHILE:
                case OPERATOR_ELSE:
                case OPERATOR_OPEN_BRACKET:
                case OPERATOR_CLOSE_BRACKET:
                case OPERATOR_OPEN_FIGURE:
                case OPERATOR_CLOSE_FIGURE:
                case OPERATOR_FINISH_SYMBOL:
                case WRONG_OPERATOR:
                default: break;
            }
        case WRONG_TYPE: break;
        default: break;
    }

    return NAN;
}