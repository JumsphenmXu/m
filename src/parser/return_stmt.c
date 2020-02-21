#include "../include/m.h"

struct return_stmt *parse_return_stmt(struct parser *p, struct token *tk) {
    struct return_stmt *st;

    st = (struct return_stmt *) malloc (sizeof(struct return_stmt));
    CHECK_NULL(st);
    SET_NODE_TYPE(st, N_STMT_RET);
    st->tk = tk;
    advance_token(p);
    st->value = parse_expr(p, P_LOWEST);
    CHECK_NULL(st->value);
    HANDLE_TRAILING(p, tk)

    return st;
}
