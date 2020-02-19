#include "../include/m.h"

struct expr_list *new_expr_list(unsigned int init_cap) {
    struct expr_list *list;
    unsigned int cap;

    cap = init_cap;
    if (cap <= 0) {
        cap = INIT_EXPR_LIST_SIZE;
    }

    list = (struct expr_list *) malloc (sizeof (struct expr_list));
    CHECK_NULL(list);

    list->exprs = (struct expr **) malloc (cap * sizeof (struct expr *));
    CHECK_NULL(list->exprs);
    list->size = 0;
    list->cap = cap;

    return list;
}

struct expr *expr_list_get(struct expr_list *list, unsigned int i) {
    if (list == NULL || i >= list->size) {
        return NULL;
    }
    return list->exprs[i];
}

int expr_list_add(struct expr_list *list, struct expr *e) {
    if (list == NULL) {
        RETURN_ERROR;
    }

    if (list->size >= list->cap) {
        CHECK_ERROR(expr_list_expand(list));
    }
    list->exprs[list->size++] = e;
    RETURN_OK;
}

int expr_list_expand(struct expr_list *list) {
    int new_cap, size;

    if (list == NULL) {
        RETURN_ERROR;
    }

    new_cap = list->cap * 7 / 4;
    if (new_cap == list->cap) {
        new_cap = 2 * new_cap + 1;
    }
    size = list->size;

    list->exprs = (struct expr **) realloc (list->exprs, new_cap * sizeof (struct expr *));
    CHECK_NULL_ERROR(list->exprs);
    list->cap = new_cap;
    list->size = size;

    RETURN_OK;
}

int expr_list_clean(struct expr_list *list) {
    int i;
    if (list == NULL) {
        RETURN_OK;
    }

    for (i = 0; i < list->size; i++) {
        free(list->exprs[i]);
    }

    free(list);
    RETURN_OK;
}
