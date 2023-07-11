#ifndef PRINT_MODES_H
#define PRINT_MODES_H
#include "arguments.h"
#include "tagged_list.h"

typedef void (*printer)(const tagged_list *);

printer get_print_mode(enum print_mode pm);
#endif
