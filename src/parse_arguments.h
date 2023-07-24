#ifndef PARSE_ARGUMENTS_H
#define PARSE_ARGUMENTS_H

#include "arguments.h"
#include "errors.h"
#include "settings.h"

enum error parse_arguments(settings_t *settings, int argc, char **argv);
#endif
