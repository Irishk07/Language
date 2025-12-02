#include <stdlib.h>

#include "array.h"
#include "common.h"
#include "differentiator.h"
#include "tree.h"
#include "start_lang.h"

int main(int, char** argv) {
    Tree_status status = SUCCESS;

    const char* html_dump_filename = argv[1];
    const char* directory          = argv[2];
    const char* file               = argv[3];

    Differentiator differentiator = {};

    DifferentiatorCtor(&differentiator, html_dump_filename, directory);

    differentiator.tree.root = LangGetComandir(&differentiator, &status, file);

    TreeHTMLDump(&differentiator, differentiator.tree.root, DUMP_INFO, NOT_ERROR_DUMP);

    DifferentiatorDtor(&differentiator);

    return 0;
}