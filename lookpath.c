#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>

/* NOTE: i have no clue if this works on windows */
#ifdef __WIN32
#define PATH_SEPARATOR ";"
#else
#define PATH_SEPARATOR ":"
#endif

struct tag {
    char *name;
    size_t position;
    size_t length;
};

typedef struct tag tagged_list;

bool matches_pattern(const char *fname,
                     const char *pattern,
                     size_t pattern_len) {
    const size_t fname_len = strlen(fname);
    uint64_t packed_fname = fname[0];
    uint64_t packed_pattern = pattern[0];

    if (fname_len < pattern_len) return false;

    /* for each 1 < pattern_len < 4; append the character to a 64 bit integer
     * which cantains the first char of fname, pattern */
    switch (pattern_len) {
    case 4:
        packed_fname   |= (fname[3] << 24);
        packed_pattern |= (pattern[3] << 24);
        /* fall through */
    case 3:
        packed_fname   |= (fname[2] << 16);
        packed_pattern |= (pattern[2] << 16);
        /* fall through */
    case 2:
        packed_fname   |= (fname[1] << 8);
        packed_pattern |= (pattern[1] << 8);
        /* fall through */
    case 1:
        return packed_fname == packed_pattern;
    default: {
        /* TODO: apply the above compression technique for the loop, hopefully
         * outside of the switch */
        size_t idx;
        for (idx = 0; idx < pattern_len; idx++) {
            if (pattern[idx] != fname[idx]) {
                return false;
            }
        }

        return true;
    }
    }
}

/* PERF: pls optimize */
/* - TODO: use geometric resize */
int add_to_matchlist(char ***matches_list, size_t matches_len, char *filename) {
    size_t filename_len = strlen(filename);
    char *buffer_fname = NULL;

    char **matches_resize =
        realloc(*matches_list, sizeof(*matches_list) * (matches_len + 1));
    if (matches_resize == NULL) {
        fprintf(stderr, "failed to resize list of matches\n");
        return -1;
    }

    buffer_fname = malloc(filename_len + 1);
    if (buffer_fname == NULL) {
        fprintf(stderr, "failed to allocate buffer for string\n");
        free(matches_resize);
        return -1;
    }

    /* set the buffer_fname to filename */
    memcpy(buffer_fname, filename, filename_len);
    buffer_fname[filename_len] = '\0';

    matches_resize[matches_len] = buffer_fname;
    *matches_list = matches_resize;

    return matches_len + 1;
}

int update_matches(const char *path,
                   const char *pattern,
                   const size_t pattern_len,
                   char ***matches_list,
                   int num_matches) {
    DIR *dir = opendir(path);
    struct dirent *entry = NULL;

    if (dir == NULL) {
        /* when the path doesn't exist in the $PATH, just skip */
        return num_matches;
    }

    while ((entry = readdir(dir))) {
        if (matches_pattern(entry->d_name, pattern, pattern_len)) {
            num_matches =
                add_to_matchlist(matches_list, num_matches, entry->d_name);
            if (num_matches < 0) {
                closedir(dir);
                return -1;
            }
        }
    }
    closedir(dir);

    return num_matches;
}

tagged_list *add_tag(tagged_list *tags,
                     size_t new_size,
                     char *name,
                     size_t pos) {
    size_t idx = new_size - 1;
    size_t name_len = strlen(name);
    tags = realloc(tags, sizeof(*tags) * new_size);

    if (tags == NULL) {
        return NULL;
    }

    tags[idx].name = malloc(name_len + 1);
    if (name == NULL) {
        free(tags);
        return NULL;
    }

    memcpy(tags[idx].name, name, name_len);
    tags[idx].name[name_len] = '\0';

    tags[idx].position = pos;
    tags[idx].length = 0;

    return tags;
}

void dump_tags(tagged_list *tags, char **elems, const size_t count) {
    struct tag *it;

    for (it = tags; it < &tags[count]; it++) {
        size_t idx = 0;
        /* when no matches in tag, length == position */
        for (idx = it->position; idx < it->length; idx++) {
            printf("%s/%s\n", it->name, elems[idx]);
            free(elems[idx]);
        }

        free(it->name);
    }

    free(tags);
    free(elems);
}

/* TODO: make this simpler */
void free_tags_in_list(tagged_list *tags, char **elems, const size_t count) {
    struct tag *it;

    for (it = tags; it < &tags[count]; it++) {
        size_t idx = 0;
        for (idx = it->position; idx < it->length; idx++) {
            free(elems[idx]);
        }
        free(it->name);
    }
    free(tags);
    free(elems);
}

int tag_each_match(char *env_path,
                   const char *pattern,
                   char ***matches,
                   tagged_list **tags) {
    char *path, *remaining;
    size_t pattern_len = strlen(pattern);

    size_t num_tags = 0;
    int num_matches = 0;

    path = strtok_r(env_path, PATH_SEPARATOR, &remaining);
    while (path != NULL) {
        tagged_list *resized_tags =
            add_tag(*tags, num_tags + 1, path, num_matches);
        if (resized_tags == NULL) {
            fprintf(stderr, "failed to reallocate list");
            free(*matches);
            *matches = NULL;
            return num_tags;
        }

        num_matches =
            update_matches(path, pattern, pattern_len, matches, num_matches);
        if (num_matches < 0) {
            free(*matches);
            *matches = NULL;
            return num_tags;
        }

        resized_tags[num_tags++].length = num_matches;
        *tags = resized_tags;
        path = strtok_r(NULL, PATH_SEPARATOR, &remaining);
    }

    return num_tags;
}

int main(int argc, char *argv[]) {
    char *env_path = NULL;

    int num_tags = 0;
    tagged_list *tags = NULL;

    char **matches = NULL;

    if (argc < 2) {
        fprintf(stderr, "%s requires an argument [pattern]\n", argv[0]);
        return EXIT_FAILURE;
    }

    env_path = getenv("PATH");
    if (env_path == NULL) {
        fprintf(stderr, "PATH variable not set, quitting\n");
        return EXIT_FAILURE;
    }

    num_tags = tag_each_match(env_path, argv[1], &matches, &tags);
    if (matches == NULL) {
        free_tags_in_list(tags, matches, num_tags);
        return EXIT_FAILURE;
    }
    dump_tags(tags, matches, num_tags);
    return EXIT_SUCCESS;
}
