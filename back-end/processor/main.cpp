#include "processor.h"

int main() {
    Processor processor = {};
    CHECK_AND_RETURN_ERRORS_PROC(ProcCtor(&processor, "byte_code.txt"));

    CHECK_AND_RETURN_ERRORS_PROC(SPU(&processor),       ProcDtor(&processor));

    // ProcDump(&processor, 0, DUMP_VAR_INFO);

    CHECK_AND_RETURN_ERRORS_PROC(ProcDtor(&processor));

    return 0;
}