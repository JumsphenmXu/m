#include "../include/m.h"

static struct if_stmt *partial_parse_if_stmt(struct parser *p, struct token *tk);

struct if_stmt *parse_if_stmt(struct parser *p, struct token *tk) {
    struct if_stmt *if_st, *elif_st;
    struct token *t_else;

    EXPECT_TOKEN(tk, TK_IF);
    if_st = partial_parse_if_stmt(p, tk);
    CHECK_NULL(if_st);
    SET_NODE_TYPE(if_st, N_STMT_IF);

    tk = get_current_token(p);
    while (IS_TOKEN(tk, TK_ELSEIF)) {
        if (if_st->elif_stmts == NULL) {
            if_st->elif_stmts = new_stmt_list(6);
            CHECK_NULL(if_st->elif_stmts);
        }

        elif_st = partial_parse_if_stmt(p, tk);
        CHECK_NULL(elif_st);
        if (stmt_list_add(if_st->elif_stmts, (struct stmt *) elif_st) < 0) {
            return NULL;
        }
        tk = get_current_token(p);
    }

    if (IS_TOKEN(tk, TK_ELSE)) {
        t_else = tk;
        advance_token(p);
        tk = get_current_token(p);
        EXPECT_TOKEN(tk, TK_LBR);
        if_st->_else = parse_block_stmt(p, tk);
        CHECK_NULL(if_st->_else);
        if_st->_else->tk = t_else;
    }

    return if_st;
}

static struct if_stmt *partial_parse_if_stmt(struct parser *p, struct token *tk) {
    struct if_stmt *if_st;

    if_st = (struct if_stmt *) malloc (sizeof (struct if_stmt));
    CHECK_NULL(if_st);
    if_st->elif_stmts = NULL;
    if_st->_else = NULL;
    if_st->tk = tk; // if or elif
    advance_token(p);

    tk = get_current_token(p);
    EXPECT_TOKEN(tk, TK_LPAR);
    if_st->lp = tk; // (
    advance_token(p);

    if_st->cond = parse_expr(p, P_LOWEST);
    CHECK_NULL(if_st->cond);

    advance_token(p);
    tk = get_current_token(p);
    EXPECT_TOKEN(tk, TK_RPAR);
    if_st->rp = tk; // )

    advance_token(p);
    tk = get_current_token(p);
    EXPECT_TOKEN(tk, TK_LBR); // {
    if_st->then = parse_block_stmt(p, tk);
    CHECK_NULL(if_st->then);
    tk = get_current_token(p);
    EXPECT_TOKEN(tk, TK_RBR);
    tk = peek_next_token(p);
    if (IS_TOKEN(tk, TK_ELSEIF) || IS_TOKEN(tk, TK_ELSE)) {
        advance_token(p);
    }

    return if_st;
}
