#include "../include/m.h"

struct expr *parse_call_expr(struct parser *p, struct expr *e) {
    struct call_expr *ce;
    struct token *tk;

    if (IS_NODE_TYPE(e, N_EXPR_LITERAL)) {
        printf("literal expression can not be called, %s\n", get_token_info(e->tk));
        return NULL;
    }

    ce = (struct call_expr *) malloc (sizeof (struct call_expr));
    CHECK_NULL(ce);
    SET_NODE_TYPE(ce, N_EXPR_CALL);
    ce->fn = e;
    ce->params = NULL;

    tk = get_current_token(p);
    EXPECT_TOKEN(tk, TK_LPAR);
    ce->lp = tk;
    advance_token(p);

    tk = get_current_token(p);
    if (!IS_TOKEN(tk, TK_RPAR)) {
        ce->params = new_expr_list(4);
        CHECK_NULL(ce->params);
    }
    while (!IS_TOKEN(tk, TK_RPAR)) {
        e = parse_expr(p, P_LOWEST);
        CHECK_NULL(e);
        if (expr_list_add(ce->params, e) < 0) {
            return NULL;
        }
        advance_token(p);
        tk = get_current_token(p);
        if (!IS_TOKEN(tk, TK_RPAR)) {
            EXPECT_TOKEN(tk, TK_COMMA);
            advance_token(p);
            tk = get_current_token(p);
        }
    }
    EXPECT_TOKEN(tk, TK_RPAR);
    ce->rp = tk;

    return (struct expr *) ce;
}
