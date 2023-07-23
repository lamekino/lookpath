#ifndef UPDATE_MATCHES_H
#define UPDATE_MATCHES_H
#include <stddef.h>

#include "settings.h"

size_t update_matches(size_t num_matches,
                      char ***matches_list,
                      const settings_t *settings,
                      const char *path);
#endif
