#include "../include/m.h"

struct for_stmt *parse_for_stmt(struct parser *p, struct token *tk) {
    struct for_stmt *fst;
    struct expr *e;

    EXPECT_TOKEN(tk, TK_FOR);
    fst = (struct for_stmt *) malloc (sizeof (struct for_stmt));
    CHECK_NULL(fst);
    SET_NODE_TYPE(fst, N_STMT_FOR);
    fst->tk = tk;
    advance_token(p);

    tk = get_current_token(p);
    EXPECT_TOKEN(tk, TK_LPAR);
    fst->lp = tk;
    advance_token(p);

    fst->init = new_expr_list(1);
    CHECK_NULL(fst->init);
    tk = get_current_token(p);
    if (!IS_TOKEN(tk, TK_SEMICOLON)) {
        do {
            e = parse_expr(p, P_LOWEST);
            CHECK_NULL(e);
            advance_token(p);
            if (expr_list_add(fst->init, e) < 0) {
                return NULL;
            }
            tk = get_current_token(p);
            if (!IS_TOKEN(tk, TK_SEMICOLON)) {
                EXPECT_TOKEN(tk, TK_COMMA);
                advance_token(p); // skip comma
                tk = get_current_token(p);
            }
        } while (!IS_TOKEN(tk, TK_SEMICOLON));
    }

    EXPECT_TOKEN(tk, TK_SEMICOLON); // 1st semicolon
    advance_token(p);
    tk = get_current_token(p);
    if (!IS_TOKEN(tk, TK_SEMICOLON)) {
        fst->cond = parse_expr(p, P_LOWEST);
        advance_token(p);
    }

    tk = get_current_token(p);
    EXPECT_TOKEN(tk, TK_SEMICOLON); // 2nd semicolon
    advance_token(p);

    fst->post = new_expr_list(1);
    CHECK_NULL(fst->post);
    tk = get_current_token(p);
    while (tk->type != TK_RPAR) {
        e = parse_expr(p, P_LOWEST);
        CHECK_NULL(e);
        advance_token(p);
        if (expr_list_add(fst->post, e) < 0) {
            return NULL;
        }
        tk = get_current_token(p);
        if (tk->type != TK_RPAR) {
            EXPECT_TOKEN(tk, TK_COMMA);
            advance_token(p); // skip comma
            tk = get_current_token(p);
        }
    }
    EXPECT_TOKEN(tk, TK_RPAR);
    advance_token(p);

    tk = get_current_token(p);
    EXPECT_TOKEN(tk, TK_LBR);
    fst->stmts = parse_block_stmt(p, tk);
    tk = get_current_token(p);
    EXPECT_TOKEN(tk, TK_RBR);
    // advance_token(p);
    return fst;
}
