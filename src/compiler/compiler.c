#include "../include/m.h"

struct compiler *new_compiler() {
    struct compiler *c;
    c = (struct compiler *) malloc (sizeof (struct compiler));
    CHECK_NULL(c);

    return c;
}

int init_compiler(struct compiler *c) {
    RETURN_OK;
}
