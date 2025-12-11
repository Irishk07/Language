#ifndef DIFFERENTIATOR_H_
#define DIFFERENTIATOR_H_

#define ADD +
#define SUB -
#define MUL *
#define POW ^
#define DIV /

#include "../common.h"
#include "language.h"


const double EPS = 1e-10;


void OptimizationTree(Language* language, Tree_node** old_node);

void OptimizationNode(Language* language, Tree_node** old_node);

void OptimizationZeroNode(Language* language, Tree* tree, Tree_node** tree_node, Tree_node* first_child, Tree_node* second_child);

void OptimizationOneNode(Language* language, Tree_node** tree_node, Tree_node* first_child, Tree_node* second_child);

bool IsConstantNode(Language* language, Tree_node* node);

bool IsZeroNode(Tree_node* node);

bool IsOneNode(Tree_node* node);

double Calculating(Language* language, Tree_node* tree_node);

#endif // DIFFERENTIATOR_H_