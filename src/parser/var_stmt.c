#include "../include/m.h"

struct var_stmt *parse_var_stmt(struct parser *p, struct token *tk) {
    struct expr *value;
    struct var_stmt *st;

    st = (struct var_stmt *) malloc (sizeof(struct var_stmt));
    CHECK_NULL(st);
    SET_NODE_TYPE(st, N_STMT_VAR);

    st->tk = tk; // var or val
    if (IS_TOKEN(tk, TK_VAL)) {
        SET_IMMUTABLE(st);
    } else {
        SET_MUTABLE(st);
    }

    advance_token(p);
    tk = get_current_token(p);
    EXPECT_TOKEN(tk, TK_IDENT);
    st->ident = tk; // ident

    advance_token(p);
    tk = get_current_token(p); // =
    EXPECT_TOKEN(tk, TK_ASSIGN);

    advance_token(p);
    st->value = parse_expr(p, P_LOWEST);
    CHECK_NULL(st->value);
    HANDLE_TRAILING(p, tk)
    return st;
}
