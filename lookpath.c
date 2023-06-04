#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>

#ifdef DEBUG
#include <assert.h>
#define ASSERT(p) assert((p))
#else
#define ASSERT(_)
#endif

bool matches_pattern(const char *fname,
                     const char *pattern,
                     size_t pattern_len) {
    const size_t fname_len = strlen(fname);
    size_t idx;
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
        /* TODO: apply the above compression technique for the loop, hopefully
         * outside of the switch */
        for (idx = 0; idx < pattern_len; idx++) {
            if (pattern[idx] != fname[idx]) {
                return false;
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

    char **matches_resize = realloc(*matches_list,
            sizeof(*matches_list) * (matches_len + 1));
    if (matches_resize == NULL) {
        return -1;
    }

    buffer_fname = malloc(filename_len + 1);
    if (buffer_fname == NULL) {
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
                break;
            }
        }
    }
    closedir(dir);

    return num_matches;
}

int main(int argc, char *argv[]) {
    char *env_path = NULL;
    char *path = NULL, *remaining = NULL;

    char **matches = NULL;
    int num_matches = 0;

    char *pattern = NULL;
    size_t pattern_len = 0;

    int idx;

    if (argc < 2) {
        fprintf(stderr, "%s requires an argument [pattern]\n", argv[0]);
        return EXIT_FAILURE;
    }

    env_path = getenv("PATH");
    if (env_path == NULL) {
        fprintf(stderr, "PATH variable not set, quitting\n");
        return EXIT_FAILURE;
    }

    pattern = argv[1];
    pattern_len = strlen(pattern);

    /* TODO: make multiple thread workers to retrieve the list of matches */
    path = strtok_r(env_path, ":", &remaining);
    while (path != NULL) {
        /* TODO: make regex support (-R? -E?) */
        num_matches =
            update_matches(path, pattern, pattern_len, &matches, num_matches);
        path = strtok_r(NULL, ":", &remaining);
    }

    /* FIXME: if num_matches < 0, this won't be freed better use a separate
     * counter for errors or find another method for errors */
    /* NOTE: this separate counter could be used to accomplish geometric
     * resize */
    for (idx = 0; idx < num_matches; idx++) {
        puts(matches[idx]);
        free(matches[idx]);
    }

    free(matches);
    return EXIT_SUCCESS;
}
