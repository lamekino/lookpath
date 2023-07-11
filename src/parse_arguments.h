#ifndef PARSE_ARGUMENTS_H
#define PARSE_ARGUMENTS_H
#include "arguments.h"
#include "settings.h"

#if 0
const char *flag_descs[] = {
    [SORTED] = "prints filenames as a sorted list (default)",
    [FULL_PATH] = "prints full paths, unsorted as given by readdir(2)",
    [FULL_PATH_SORTED] = "prints full path sorted by basename",
    [TREE] = "prints matches as a tree, unsorted, as given by readdir(2)",
    [TREE_SORTED] = "prints as a tree with sorted elements",
    [TREE_SORTED + 1] = "print this help"
};
#endif

int parse_arguments(settings_t *settings, int argc, char **argv);
#endif
