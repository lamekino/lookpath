#ifndef TAG_MATCHES_H
#define TAG_MATCHES_H
#include "settings.h"
#include "tagged_list.h"

int tag_matches(const settings_t *settings,
                tagged_list *tags,
                char *env_path);
#endif
