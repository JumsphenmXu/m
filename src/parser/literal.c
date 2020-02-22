#include "../include/m.h"

struct expr *parse_ident_expr(struct parser *p) {
    struct ident_expr *e;
    struct token *tk;

    e = (struct ident_expr*) malloc (sizeof (struct ident_expr));
    CHECK_NULL(e);
    SET_NODE_TYPE(e, N_EXPR_IDENT);

    tk = get_current_token(p);
    CHECK_NULL(e);
    e->tk = tk;
    if (tk->type == TK_VAL) {
        SET_IMMUTABLE(e);
    }

    return (struct expr*) e;
}

struct expr *parse_literal_expr(struct parser *p) {
    struct literal_expr *e;
    struct token *tk;
    int base;

    e = (struct literal_expr *) malloc (sizeof (struct literal_expr));
    CHECK_NULL(e);
    SET_NODE_TYPE(e, N_EXPR_LITERAL);

    tk = get_current_token(p);
    e->tk = tk;
    SET_IMMUTABLE(e);

    switch (tk->type) {
        case TK_NIL:
            break;
        case TK_CHAR:
            e->value.c = tk->literal[0];
            break;
        case TK_INT:
            if (strncmp(tk->literal, "0x", 2) == 0 || strncmp(tk->literal, "0X", 2) == 0) {
                base = 16;
            } else {
                base = 10;
            }
            e->value.l = strtol(tk->literal, NULL, base);
            break;
        case TK_FLOAT:
            e->value.f = strtod(tk->literal, NULL);
            break;
        case TK_STRING:
            e->value.s = tk->literal;
            break;
        case TK_TRUE:
        case TK_FALSE:
            e->value.b = tk->type == TK_TRUE ? 1 : 0;
            break;
        default:
            printf("parse literal expression, unknown %s\n", get_token_info(tk));
            break;
    }

    return (struct expr *) e;
}
