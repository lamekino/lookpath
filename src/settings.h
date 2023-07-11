#ifndef SETTINGS_H
#define SETTINGS_H
#include <stdbool.h>

#include "arguments.h"

typedef struct {
    enum print_mode print_mode;
    char *pattern;
} settings_t;

#endif
