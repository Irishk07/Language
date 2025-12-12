#include <stdlib.h>

#include "array.h"
#include "../common.h"
#include "middle_end.h"
#include "tree.h"


int main(int argc, char** argv) {
    const char* html_dump_filename = "dump.html";
    const char* directory          = "dump";
    const char* tree_file          = "../tree.txt";

    if (argc >= 2)
        html_dump_filename = argv[1];
    if (argc >= 3)
        directory = argv[2];
    if (argc >= 4)
        tree_file = argv[3];

    Language language = {};

    LanguageCtor(&language, html_dump_filename, directory, tree_file);

    Middle_end(&language);

    TreeHTMLDump(&language, language.tree.root, DUMP_INFO, NOT_ERROR_DUMP);

    LanguageDtor(&language);

    return 0;
}