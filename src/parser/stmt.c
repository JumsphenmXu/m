#include "../include/m.h"

struct stmt *new_stmt() {
    struct stmt *st;
    st = (struct stmt *) malloc (sizeof(struct stmt));
    CHECK_NULL(st);
    return st;
}

struct stmt *parse_stmt(struct parser *p, struct token *tk) {
    struct stmt *st;

    switch (tk->type) {
        case TK_VAR:
        case TK_VAL:
            st = (struct stmt *) parse_var_stmt(p, tk);
            break;
        case TK_RET:
            st = (struct stmt *) parse_return_stmt(p, tk);
            break;
        /*
        case TK_LBR:
            st = (struct stmt *) parse_block_stmt(p, tk);
            break;
        */
        case TK_IF:
            st = (struct stmt *) parse_if_stmt(p, tk);
            break;
        case TK_FOR:
            st = (struct stmt *) parse_for_stmt(p, tk);
            break;
        default:
            st = (struct stmt *) parse_expr_stmt(p);
            break;
    }

    return st;
}

struct stmt_list *new_stmt_list(unsigned int stmt_num) {
    struct stmt_list *list;
    unsigned int cap;

    cap = stmt_num;
    if (cap <= 0) {
        cap = INIT_STMT_LIST_SIZE;
    }

    list = (struct stmt_list *) malloc (sizeof (struct stmt_list));
    CHECK_NULL(list);

    list->stmts = (struct stmt **) malloc (cap * sizeof (struct stmt *));
    CHECK_NULL(list->stmts);
    list->size = 0;
    list->cap = cap;

    return list;
}

struct stmt *stmt_list_get(struct stmt_list *list, int i) {
    if (list == NULL || i >= list->size) {
        return NULL;
    }
    return list->stmts[i];
}

int stmt_list_add(struct stmt_list *list, struct stmt *st) {
    if (list == NULL) {
        RETURN_ERROR;
    }

    if (list->size >= list->cap) {
        CHECK_ERROR(stmt_list_expand(list));
    }
    list->stmts[list->size++] = st;
    RETURN_OK;
}

int stmt_list_expand(struct stmt_list *list) {
    int new_cap, size;

    if (list == NULL) {
        RETURN_ERROR;
    }

    new_cap = list->cap * 7 / 4;
    if (new_cap == list->cap) {
        new_cap = 2 * new_cap + 1;
    }
    size = list->size;

    list->stmts = (struct stmt **) realloc (list->stmts, new_cap * sizeof (struct stmt *));
    CHECK_NULL_ERROR(list->stmts);
    list->cap = new_cap;
    list->size = size;

    RETURN_OK;
}

int stmt_list_clean(struct stmt_list *list) {
    int i;
    if (list == NULL) {
        RETURN_OK;
    }

    for (i = 0; i < list->size; i++) {
        free(list->stmts[i]);
    }

    free(list);
    RETURN_OK;
}
