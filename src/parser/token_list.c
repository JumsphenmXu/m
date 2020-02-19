#include "../include/m.h"

struct token_list *new_token_list(unsigned int init_cap) {
    struct token_list *list;
    unsigned int cap;

    cap = init_cap;
    if (cap <= 0) {
        cap = INIT_TOKEN_LIST_SIZE;
    }

    list = (struct token_list *) malloc (sizeof (struct token_list));
    CHECK_NULL(list);

    list->tokens = (struct token **) malloc (cap * sizeof (struct token *));
    CHECK_NULL(list->tokens);
    list->size = 0;
    list->cap = cap;

    return list;
}

struct token *token_list_get(struct token_list *list, unsigned int i) {
    if (list == NULL || i >= list->size) {
        return NULL;
    }
    return list->tokens[i];
}

int token_list_add(struct token_list *list, struct token *tk) {
    if (list == NULL) {
        RETURN_ERROR;
    }

    if (list->size >= list->cap) {
        CHECK_ERROR(token_list_expand(list));
    }
    list->tokens[list->size++] = tk;
    RETURN_OK;
}

int token_list_expand(struct token_list *list) {
    int new_cap, size;

    if (list == NULL) {
        RETURN_ERROR;
    }

    new_cap = list->cap * 7 / 4;
    if (new_cap == list->cap) {
        new_cap = 2 * new_cap + 1;
    }
    size = list->size;

    list->tokens = (struct token **) realloc (list->tokens, new_cap * sizeof (struct token *));
    CHECK_NULL_ERROR(list->tokens);
    list->cap = new_cap;
    list->size = size;

    RETURN_OK;
}

int token_list_clean(struct token_list *list) {
    int i;
    if (list == NULL) {
        RETURN_OK;
    }

    for (i = 0; i < list->size; i++) {
        free(list->tokens[i]);
    }

    free(list);
    RETURN_OK;
}
