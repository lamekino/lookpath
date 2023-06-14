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

typedef struct {
    struct tag *tags;
    char **strings;

    size_t num_strings;
    size_t num_tags;
} tagged_list;

enum error {
    FAILED_BUFFER_ALLOC = INT_MIN,
    FAILED_BUFFER_REALLOC,
    NO_PATH_VAR,
    EMPTY_PATH_VAR,
    INSUFFICIENT_ARGS,
    WRONG_ARGS,
    DOUBLE_PATTERN,
    NO_PATTERN,

    NO_ERROR = 0
};
#define HAS_ERROR(p) ((p) != NO_ERROR)

enum print_mode {
    SORTED,
    FULL_PATH,
    FULL_PATH_SORTED, /* maybe */
    TREE
};

#define FLAG_SORTED "-S"
#define FLAG_FULL_PATH "-f"
#define FLAG_FULL_PATH_SORTED "-F"
#define FLAG_TREE "-t"

typedef struct {
    enum print_mode print_mode;
    char *pattern;
} settings_t;

int what_happened(enum error code, const char *prog_name) {
    (void) prog_name;
    switch (code) {
    case FAILED_BUFFER_ALLOC:
        perror("issue with malloc");
        fprintf(stderr, "failed to allocate buffer\n");
        break;
    case FAILED_BUFFER_REALLOC:
        perror("issue with realloc");
        fprintf(stderr, "failed to reallocate buffer\n");
        break;
    case NO_PATH_VAR:
        fprintf(stderr, "PATH variable not set, quitting\n");
        break;
    case EMPTY_PATH_VAR:
        fprintf(stderr, "PATH is empty string, qutting\n");
        break;
    case INSUFFICIENT_ARGS:
        fprintf(stderr, "requires a 1 argument [pattern]\n");
        break;
    case WRONG_ARGS:
        fprintf(stderr, "mismatched or invalid argument provided, quitting\n");
        break;
    case DOUBLE_PATTERN:
        fprintf(stderr, "can only take one pattern, quitting\n");
        break;
    case NO_PATTERN:
        fprintf(stderr, "no pattern provided in arguments\n");
        break;
    default:
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}

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
    default:
    {
        /* TODO: apply the above compression technique for the loop, hopefully
         * outside of the switch */
        size_t idx;
        for (idx = 0; idx < pattern_len; idx++) {
            if (pattern[idx] != fname[idx]) {
                return false;
            }
        }
    }
    return true;
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
        return FAILED_BUFFER_REALLOC;
    }

    buffer_fname = calloc(1, NAME_MAX + 1);
    if (buffer_fname == NULL) {
        free(matches_resize);
        return FAILED_BUFFER_ALLOC;
    }

    /* set the buffer_fname to filename */
    /* TODO: look into strdup(3) */
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
            int result =
                add_to_matchlist(matches_list, num_matches, entry->d_name);
            if (num_matches < 0) {
                closedir(dir);
                return result;
            }

            num_matches = result;
        }
    }
    closedir(dir);

    return num_matches;
}

tagged_list *add_tag(tagged_list *ts,
                     char *name,
                     size_t position,
                     size_t length) {
    struct tag *new_tag = NULL;
    struct tag *realloc_tags = NULL;
    size_t name_len = strlen(name);

    realloc_tags =
        realloc(ts->tags, sizeof(*ts->tags) * (ts->num_tags + 1));
    if (realloc_tags == NULL) {
        return NULL;
    }
    ts->tags = realloc_tags;
    new_tag = &realloc_tags[ts->num_tags];

    new_tag->name = malloc(name_len + 1);
    if (new_tag->name == NULL) {
        free(ts->tags);
        return NULL;
    }

    memcpy(new_tag->name, name, name_len);
    new_tag->name[name_len] = '\0';

    new_tag->position = position;
    new_tag->length = length;
    ts->num_tags++;

    return ts;
}

int parse_arguments(settings_t *settings, int argc, char **argv) {
    const size_t flag_cap = 2;

    int idx;
    for (idx = 1; idx < argc; idx++) {
        if (argv[idx][0] != '-') {
            if (settings->pattern != NULL) {
                return DOUBLE_PATTERN;
            }
            settings->pattern = argv[idx];
            continue;
        }

        if (strncmp(argv[idx], FLAG_SORTED, flag_cap) == 0) {
            settings->print_mode = SORTED;
            continue;
        }

        if (strncmp(argv[idx], FLAG_FULL_PATH, flag_cap) == 0) {
            settings->print_mode = FULL_PATH;
            continue;
        }

        if (strncmp(argv[idx], FLAG_FULL_PATH_SORTED, flag_cap) == 0) {
            settings->print_mode = FULL_PATH_SORTED;
            continue;
        }

        if (strncmp(argv[idx], FLAG_TREE, flag_cap) == 0) {
            settings->print_mode = TREE;
            continue;
        }

        return WRONG_ARGS;
    }

    if (settings->pattern == NULL) {
        return NO_PATTERN;
    }

    return NO_ERROR;
}

void free_tags(tagged_list *t) {
    while (t->num_strings--) {
        free(t->strings[t->num_strings]);
    }
    while (t->num_tags--) {
        free(t->tags[t->num_tags].name);
    }

    free(t->tags);
    free(t->strings);
}

int tag_each_match(char *env_path,
                   const char *pattern,
                   tagged_list *tags) {
    char *path, *remaining;
    size_t pattern_len = strlen(pattern);

    size_t num_tags = 0;
    int num_matches = 0;

    for (
            path = strtok_r(env_path, PATH_SEPARATOR, &remaining);
            path != NULL;
            path = strtok_r(NULL, PATH_SEPARATOR, &remaining)
    ) {
        int result = 0;
        tagged_list *resized_tags = NULL;

        /* TODO: better name for result */
        result =
            update_matches(path, pattern, pattern_len, &tags->strings, num_matches);
        if (result < 0) {
            return result;
        }

        resized_tags = add_tag(tags, path, num_matches, result);
        if (resized_tags == NULL) {
            return FAILED_BUFFER_REALLOC;
        }
        tags = resized_tags;
        tags->num_strings = result;
        num_matches = result;
    }

    return num_tags;
}

int compare(const void *a, const void *b) {
    return strncmp(*((const char **) a), *((const char **) b), NAME_MAX);
}

bool has_matches(const struct tag *tag) {
    /* when no matches in tag, length == position */
    return tag->length != tag->position;
}

void do_settings(const settings_t *settings, tagged_list *tags) {
    switch (settings->print_mode) {
    /* TODO: MAKE THESE FUNCTIONS! */
    case SORTED:
    {
        size_t i;
        qsort(tags->strings, tags->num_strings, sizeof(char *), &compare);
        for (i = 0; i < tags->num_strings; i++) {
            puts(tags->strings[i]);
        }
    }
    break;
    case FULL_PATH:
    {
        struct tag *it;

        for (it = tags->tags; it <= &tags->tags[tags->num_tags - 1]; it++) {
            size_t idx = 0;

            /* when no matches in tag, length == position */
            for (idx = it->position; idx < it->length; idx++) {
                printf("%s/%s\n", it->name, tags->strings[idx]);
            }
        }
    }
    break;
    case FULL_PATH_SORTED:
    {
        struct tag *it;
        for (it = tags->tags; it <= &tags->tags[tags->num_tags - 1]; it++) {
            size_t idx;
            if (!has_matches(it)) {
                continue;
            }
            qsort(&tags->strings[it->position], it->length, sizeof(char *), &compare);

            for (idx = it->position; idx < it->length; idx++) {
                printf("%s %s\n", it->name, tags->strings[idx]);
            }
        }
    }
    break;
    case TREE:
    {
        struct tag *it;
        const char *POINT = "├─";
        const char *LAST_POINT = "└─";

        for (it = tags->tags; it <= &tags->tags[tags->num_tags - 1]; it++) {
            size_t idx = 0;

            if (!has_matches(it)) {
                continue;
            }
            printf("%s:\n", it->name);
            for (idx = it->position; idx < it->length; idx++) {
                /* WARN: probably wrong */
                const char *point = POINT;

                if (idx == it->length - 1) {
                    point = LAST_POINT; /* why does this work??? */
                }
                printf("%s %s\n", point, tags->strings[idx]);
            }
        }
    }
    break;
    }
}

int main(int argc, char *argv[]) {
    /* hold's the user's settings determined by program flags,
     * parse_arguments() */
    settings_t settings = {0};

    /* points to the $PATH environment variable */
    char *env_path = NULL;

    /* stores the result for error functions */
    enum error err = NO_ERROR;

    /*
     * tags.tags: the pointer to the list of tags
     * tags.num_tags: the number of tags in tags.tags
     * tags.strings: the pointer to the tagged list
     * tags.num_strings: the number of strings
     */
    tagged_list tags = {0};

    if (argc < 2) {
        return what_happened(INSUFFICIENT_ARGS, argv[0]);
    }

    env_path = getenv("PATH");
    if (env_path == NULL) {
        return what_happened(NO_PATH_VAR, argv[0]);
    }
    if (strlen(env_path) == 0) {
        return what_happened(EMPTY_PATH_VAR, argv[0]);
    }

    err = parse_arguments(&settings, argc, argv);
    if (HAS_ERROR(err)) {
        return what_happened(err, argv[0]);
    }

    err = tag_each_match(env_path, settings.pattern, &tags);
    if (!HAS_ERROR(err)) {
        do_settings(&settings, &tags);
    }

    free_tags(&tags);
    return what_happened(err, argv[0]);
}
