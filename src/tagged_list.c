#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "tagged_list.h"
#include "errors.h"

tagged_list *add_tag(tagged_list *ts,
                     char *name,
                     size_t position,
                     size_t length) {
    struct tag *new_tag = NULL;
    struct tag *realloc_tags = NULL;

    realloc_tags =
        realloc(ts->tags, sizeof(*ts->tags) * (ts->num_tags + 1));
    if (realloc_tags == NULL) {
        return NULL;
    }
    ts->tags = realloc_tags;
    new_tag = &realloc_tags[ts->num_tags];

    new_tag->name = strdup(name);
    if (new_tag->name == NULL) {
        free(ts->tags);
        return NULL;
    }

    new_tag->position = position;
    new_tag->length = length;
    ts->num_tags++;

    return ts;
}

void clear_tags(tagged_list *t) {
    while (t->num_strings--) {
        free(t->strings[t->num_strings]);
    }
    while (t->num_tags--) {
        free(t->tags[t->num_tags].name);
    }

    free(t->tags);
    free(t->strings);

    t->tags = NULL;
    t->strings = NULL;
}

bool has_matches(const struct tag *tag) {
    /* when no matches in tag, length == position */
    return tag->length != tag->position;
}
