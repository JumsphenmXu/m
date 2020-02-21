#include "../include/m.h"

struct expr *parse_index_expr(struct parser *p, struct expr *e) {
    struct index_expr *ie;
    struct token *tk;

    if (IS_NODE_TYPE(e, N_EXPR_LITERAL)) {
        printf("literal expression can not be indexed, %s\n", get_token_info(e->tk));
        exit(-1);
    }

    ie = (struct index_expr *) malloc (sizeof (struct index_expr));
    CHECK_NULL(ie);
    SET_NODE_TYPE(ie, N_EXPR_INDEX);
    ie->e = e;
    ie->start = NULL;
    ie->end = NULL;
    ie->colon = NULL;

    tk = get_current_token(p);
    EXPECT_TOKEN(tk, TK_LSQUARE);
    ie->lsq = tk;
    advance_token(p);

    tk = get_current_token(p);
    if (IS_TOKEN(tk, TK_RSQUARE)) {
        printf("`]` is not expected directly after `[`, %s\n", get_token_info(tk));
        exit(-1);
    }

    if (!IS_TOKEN(tk, TK_COLON)) {
        ie->start = parse_expr(p, P_LOWEST);
        advance_token(p);
        tk = get_current_token(p);
        if (IS_TOKEN(tk, TK_RSQUARE)) {
            goto out;
        }
    }
    EXPECT_TOKEN(tk, TK_COLON);
    ie->colon = tk;
    advance_token(p);
    tk = get_current_token(p);
    if (!IS_TOKEN(tk, TK_RSQUARE)) {
        ie->end = parse_expr(p, P_LOWEST);
        advance_token(p);
        tk = get_current_token(p);
    }

out:
    EXPECT_TOKEN(tk, TK_RSQUARE);
    ie->rsq = tk;

    return (struct expr *) ie;
}
