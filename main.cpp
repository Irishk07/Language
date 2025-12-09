#include <stdlib.h>

#include "array.h"
#include "common.h"
#include "language.h"
#include "tokenizator.h"
#include "tree.h"
#include "syntax_analize.h"

int main(int, char** argv) {
    Tree_status status = SUCCESS;

    const char* html_dump_filename = argv[1];
    const char* directory          = argv[2];
    const char* file               = argv[3];

    Language language = {};

    LanguageCtor(&language, html_dump_filename, directory);

    Tokenizator(&language, file);

    language.tree.root = LangGetComandir(&language, &status);

    CreatePreOrderTreeFile(&language, "tree.txt");

    TreeHTMLDump(&language, language.tree.root, DUMP_INFO, NOT_ERROR_DUMP);

    LanguageDtor(&language);

    return 0;
}