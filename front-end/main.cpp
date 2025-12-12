#include <stdlib.h>

#include "array.h"
#include "../common.h"
#include "front_end.h"
#include "tokenizator.h"
#include "tree.h"
#include "syntax_analize.h"


int main(int argc, char** argv) {
    Tree_status status = SUCCESS;

    const char* html_dump_filename = "dump.html";
    const char* directory          = "dump";
    const char* programm_file      = "programm.txt";
    const char* tree_file          = "../tree.txt";

    if (argc >= 2)
        html_dump_filename = argv[1];
    if (argc >= 3)
        directory = argv[2];
    if (argc >= 4)
        programm_file = argv[3];
    if (argc >= 5)
        tree_file = argv[4];

    Language language = {};

    LanguageCtor(&language, html_dump_filename, directory, programm_file, tree_file);

    Tokenizator(&language);

    language.tree.root = LangGetComandir(&language, &status);

    CreatePreOrderTreeFile(&language);

    TreeHTMLDump(&language, language.tree.root, DUMP_INFO, NOT_ERROR_DUMP);

    LanguageDtor(&language);

    return 0;
}