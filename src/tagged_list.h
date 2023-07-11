#ifndef TAGGED_LIST_H
#define TAGGED_LIST_H
#include <stddef.h>

struct tag {
    char *name;
    /* TODO: use pointers instead of indices */
#if 0
    char ***start;
    char ***end;
#else
    size_t position;
    size_t length;
#endif
};

typedef struct {
    struct tag *tags;
    char **strings;

    size_t num_strings;
    size_t num_tags;
} tagged_list;

tagged_list *add_tag(tagged_list *ts,
                     char *name,
                     size_t position,
                     size_t length);
void clear_tags(tagged_list *t);
bool has_matches(const struct tag *tag);
#endif
