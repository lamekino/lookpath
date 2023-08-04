#include "do_settings.h"
#include "arguments.h"
#include "print_modes.h"
#include "settings.h"
#include "tagged_list.h"
#include "errors.h"

int do_settings(const settings_t *settings, tagged_list *tags) {
    get_print_mode(settings->mask[PRINT_FLAGS])(tags);
    return NO_ERROR;
}

