#include <stdlib.h>

#include "array.h"
#include "common.h"
#include "language.h"
#include "tokenizator.h"
#include "tree.h"
#include "syntax_analize.h"


// функции, вызов и определение, возврат значения, где хранить названия функции
// дробление на файлы? как лучше
// как прокидывать название файла с деревом

int main(int, char** argv) {
    Tree_status status = SUCCESS;

    const char* html_dump_filename = argv[1];
    const char* directory          = argv[2];
    const char* programm_file      = argv[3];

    Language language = {};

    LanguageCtor(&language, html_dump_filename, directory);

    Tokenizator(&language, programm_file);

    language.tree.root = LangGetComandir(&language, &status);

    CreatePreOrderTreeFile(&language, programm_file);

    TreeHTMLDump(&language, language.tree.root, DUMP_INFO, NOT_ERROR_DUMP);

    Middle_end(&language, "tree_spusk.txt");

    TreeHTMLDump(&language, language.tree.root, DUMP_INFO, NOT_ERROR_DUMP);

    LanguageDtor(&language);

    return 0;
}