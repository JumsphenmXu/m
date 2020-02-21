#include "../include/m.h"

struct expr *parse_func_expr(struct parser *p, struct token *tk) {
    struct func_expr *fe;
    int n;

    EXPECT_TOKEN(tk, TK_FUNC);
    fe = (struct func_expr *) malloc (sizeof (struct func_expr));
    CHECK_NULL(fe);
    SET_NODE_TYPE(fe, N_EXPR_FUNC);
    fe->tk = tk;
    fe->params = NULL;
    advance_token(p); // skip 'func'

    tk = get_current_token(p);
    EXPECT_TOKEN(tk, TK_LPAR);
    fe->lp = tk;
    advance_token(p);

    tk = get_current_token(p);
    if (!IS_TOKEN(tk, TK_RPAR)) {
        fe->params = new_token_list(0);
        CHECK_NULL(fe->params);
    }

    while (!IS_TOKEN(tk, TK_RPAR)) {
        EXPECT_TOKEN(tk, TK_IDENT);
        if (token_list_add(fe->params, tk) < 0) {
            return NULL;
        }

        advance_token(p);
        tk = get_current_token(p);
        if (IS_TOKEN(tk, TK_COMMA)) {
            advance_token(p);
            tk = get_current_token(p);
        }
    }
    EXPECT_TOKEN(tk, TK_RPAR);
    fe->rp = tk;
    advance_token(p);

    tk = get_current_token(p);
    EXPECT_TOKEN(tk, TK_LBR);
    fe->stmts = parse_block_stmt(p, tk);

    return (struct expr *) fe;
}
