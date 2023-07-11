#ifndef TAG_MATCHES_H
#define TAG_MATCHES_H
#include "tagged_list.h"

int tag_matches(char *env_path,
                const char *pattern,
                tagged_list *tags);
#endif
