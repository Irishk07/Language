#ifndef DIFFERENTIATOR_H_
#define DIFFERENTIATOR_H_

#include "common.h"


Tree_status DifferentiatorCtor(Differentiator* differentiator, const char* html_dump_filename, const char* directory);

Tree_status DifferentiatorDtor(Differentiator* differentiator);

void DifferentiatorNodeDtor(Differentiator* differentiator, Tree_node* tree_node);


#endif // DIFFERENTIATOR_H_