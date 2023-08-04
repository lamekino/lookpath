#ifndef PRINT_MODES_H
#define PRINT_MODES_H

#include "bits.h"
#include "arguments.h"
#include "tagged_list.h"

enum print_mode {
#define PRINT_MODE_ENUM(label, ...) PM_##label,
    PRINT_FLAGS(PRINT_MODE_ENUM)
#undef PRINT_MODE_ENUM
    NUM_PRINT_MODES
};

typedef void (*print_mode_fp)(const tagged_list *);

print_mode_fp get_print_mode(const packed_t pmask);

#endif
