#ifndef TAGGED_LIST_H
#define TAGGED_LIST_H
#include <stddef.h>
#include <stdbool.h>

struct tag {
    char *name;
    size_t position;
    size_t length;
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
