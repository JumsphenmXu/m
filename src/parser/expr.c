#include "../include/m.h"

struct expr *parse_expr(struct parser *p, enum prec _prec) {
    struct expr *e;
    struct token *tk;
    enum prec pr;

    e = (struct expr *) parse_unary_expr(p);
    tk = peek_next_token(p);
    pr = get_precedence(tk);
    // printf("parse expr before while, current %s, next %s\n", get_token_info(get_current_token(p)), get_token_info(tk));
    while (!IS_TOKEN(tk, TK_SEMICOLON) && !IS_TOKEN(tk, TK_COMMA) && _prec < pr) {
        /*
        printf("current: %s, next: %s, _prec: %s, pr: %s\n",
                get_token_info(get_current_token(p)), get_token_info(tk),
                get_prec_name(_prec), get_prec_name(pr));
        */
        advance_token(p);
        tk = get_current_token(p);
        switch (tk->type) {
            case TK_LPAR:
                e = parse_call_expr(p, e);
                break;
            case TK_LSQUARE:
                e = parse_index_expr(p, e);
                break;
            default:
                e = parse_binary_expr(p, e);
                break;
        }
        tk = peek_next_token(p);
        pr = get_precedence(tk);
    }
    // printf("parse expr after while, current %s, next %s\n", get_token_info(get_current_token(p)), get_token_info(tk));

    return e;
}

struct expr *parse_unary_expr(struct parser *p) {
    struct unary_expr *ue;
    struct expr *e;
    struct token *tk;

    ue = (struct unary_expr *) malloc (sizeof (struct unary_expr));
    CHECK_NULL(ue);
    SET_NODE_TYPE(ue, N_EXPR_UNARY);
    tk = get_current_token(p);
    ue->tk = tk;
    switch (tk->type) {
        case TK_IDENT:
            e = parse_ident_expr(p);
            break;
        case TK_NIL:
        case TK_CHAR:
        case TK_INT:
        case TK_FLOAT:
        case TK_STRING:
        case TK_TRUE:
        case TK_FALSE:
            e = parse_literal_expr(p);
            break;
        case TK_NOT:
        case TK_MINUS:
        case TK_PLUS:
        case TK_NEGATE:
            advance_token(p);
            e = parse_expr(p, P_UNARY);
            ue->e = e;
            e = (struct expr *) ue;
            break;
        case TK_LPAR:
            e = parse_group_expr(p, tk);
            break;
        case TK_LBR:
            e = parse_map_expr(p, tk);
            break;
        case TK_LSQUARE:
            e = parse_array_expr(p, tk);
            break;
        case TK_FUNC:
            e = parse_func_expr(p, tk);
            break;
        default:
            printf("parse unary expression, unexpected %s\n", get_token_info(tk));
            exit(-1);
    }

    return e;
}

struct expr *parse_binary_expr(struct parser *p, struct expr *left) {
    struct token *tk;
    struct binary_expr *be;
    enum prec pr;

    be = (struct binary_expr *) malloc (sizeof(struct binary_expr));
    CHECK_NULL(be);
    SET_NODE_TYPE(be, N_EXPR_BINARY);
    be->left = left;

    tk = get_current_token(p);
    be->op = tk;
    advance_token(p);

    pr = get_precedence(tk);
    be->right = parse_expr(p, pr);

    return (struct expr *) be;
}

struct expr *parse_group_expr(struct parser *p, struct token *tk) {
    struct group_expr *ge;

    EXPECT_TOKEN(tk, TK_LPAR);
    ge = (struct group_expr *) malloc (sizeof (struct group_expr));
    CHECK_NULL(ge);
    SET_NODE_TYPE(ge, N_EXPR_GROUP);
    ge->tk = tk; // setup '('
    advance_token(p); // skip '('

    ge->e = parse_expr(p, P_LOWEST);

    advance_token(p); // skip current expr
    tk = get_current_token(p);
    EXPECT_TOKEN(tk, TK_RPAR);
    ge->rp = tk; // setup ')'

    return (struct expr *) ge;
}

struct expr_stmt *new_expr_stmt(struct expr *e) {
    struct expr_stmt *st;
    st = (struct expr_stmt *) malloc (sizeof(struct expr_stmt));
    CHECK_NULL(st);
    SET_NODE_TYPE(st, N_STMT_EXPR);
    st->e = e;
    return st;
}

struct expr_stmt *parse_expr_stmt(struct parser *p) {
    struct token *tk;
    struct expr *e;
    struct expr_stmt *st;

    st = new_expr_stmt(NULL);
    CHECK_NULL(st);
    st->tk = get_current_token(p);
    e = parse_expr(p, P_LOWEST);
    st->e = e;
    HANDLE_TRAILING(p, tk)

    return st;
}

enum prec get_precedence(struct token *tk) {
    enum prec p;

    switch (tk->type) {
        case TK_ASSIGN:
        case TK_LSHIFT_ASSIGN:
        case TK_RSHIFT_ASSIGN:
        case TK_AND_ASSIGN:
        case TK_OR_ASSIGN:
        case TK_XOR_ASSIGN:
        case TK_PLUS_ASSIGN:
        case TK_MINUS_ASSIGN:
        case TK_STAR_ASSIGN:
        case TK_SLASH_ASSIGN:
        case TK_PERCENT_ASSIGN:
            p = P_ASSIGN;
            break;
        case TK_LAND:
            p = P_LAND;
            break;
        case TK_LOR:
            p = P_LOR;
            break;
        case TK_LSHIFT:
        case TK_RSHIFT:
            p = P_SHIFT;
            break;
        case TK_EQ:
        case TK_NE:
        case TK_GT:
        case TK_GE:
        case TK_LT:
        case TK_LE:
            p = P_CMP;
            break;
        case TK_AND:
        case TK_OR:
        case TK_XOR:
            p = P_BIT_OP;
            break;
        case TK_MINUS:
        case TK_PLUS:
            p = P_SUM;
            break;
        case TK_STAR:
        case TK_SLASH:
        case TK_PERCENT:
            p = P_PRODUCT;
            break;
        case TK_NIL:
        case TK_IDENT:
        case TK_INT:
        case TK_FLOAT:
        case TK_STRING:
        case TK_TRUE:
        case TK_FALSE:
        case TK_NEGATE:
        case TK_NOT:
            p = P_UNARY;
            break;
        case TK_LSQUARE:
            p = P_CALL;
            break;
        case TK_LPAR:
            p = P_CALL;
            break;
        default:
            p = P_LOWEST;
            break;
    }

    return p;
}
