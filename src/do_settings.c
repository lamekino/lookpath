#include "do_settings.h"
#include "print_modes.h"
#include "settings.h"
#include "tagged_list.h"
#include "errors.h"

int do_settings(const settings_t *settings, tagged_list *tags) {
    get_print_mode(settings->print_mode)(tags);
    return NO_ERROR;
}

