#include "../include/m.h"

struct expr *parse_array_expr(struct parser *p, struct token *tk) {
    struct array_expr *ae;
    struct expr *e;

    EXPECT_TOKEN(tk, TK_LSQUARE);
    ae = (struct array_expr *) malloc (sizeof (struct array_expr));
    CHECK_NULL(ae);
    SET_NODE_TYPE(ae, N_EXPR_ARRAY);
    ae->tk = tk;
    ae->elems = NULL;
    advance_token(p);

    tk = get_current_token(p);
    if (!IS_TOKEN(tk, TK_RSQUARE)) {
        ae->elems = new_expr_list(6);
        CHECK_NULL(ae->elems);
    }
    while (!IS_TOKEN(tk, TK_RSQUARE)) {
        e = parse_expr(p, P_LOWEST);
        CHECK_NULL(e);
        if (expr_list_add(ae->elems, e) < 0) {
            return NULL;
        }

        advance_token(p);
        tk = get_current_token(p);
        if (!IS_TOKEN(tk, TK_RSQUARE)) {
            EXPECT_TOKEN(tk, TK_COMMA);
            advance_token(p);
            tk = get_current_token(p);
        }
    }
    EXPECT_TOKEN(tk, TK_RSQUARE);
    ae->rsq = tk;

    return (struct expr *) ae;
}
