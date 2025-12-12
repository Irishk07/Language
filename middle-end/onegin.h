#ifndef ONEGIN_H_
#define ONEGIN_H_

#include "../common.h"
#include "middle_end.h"


int SizeOfText(const char *text_name);

Tree_status ReadOnegin(Language* language, const char* name_file);

#endif // ONEGIN_H_