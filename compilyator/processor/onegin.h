#ifndef ONEGIN_H_
#define ONEGIN_H_

#include "processor.h"

int OneginTextSize(const char *text_name);

processor_status OneginReadFile(Processor* processor);

processor_status DivisionIntoCommands(Processor* processor);

#endif // ONEGIN_H_