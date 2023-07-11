#ifndef UPDATE_MATCHES_H
#define UPDATE_MATCHES_H
#include <stddef.h>

int update_matches(const char *path,
                   const char *pattern,
                   const size_t pattern_len,
                   char ***matches_list,
                   int num_matches);
#endif
