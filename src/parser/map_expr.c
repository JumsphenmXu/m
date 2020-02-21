#include "../include/m.h"

struct expr *parse_map_expr(struct parser *p, struct token *tk) {
    struct map_expr *me;
    struct expr *key, *value;

    EXPECT_TOKEN(tk, TK_LBR);
    me = (struct map_expr *) malloc (sizeof (struct map_expr));
    CHECK_NULL(me);
    SET_NODE_TYPE(me, N_EXPR_MAP);
    me->keys = NULL;
    me->values = NULL;
    me->tk = tk;

    advance_token(p);
    if (!IS_TOKEN(tk, TK_RBR)) {
        me->keys = new_expr_list(6);
        CHECK_NULL(me->keys);
        me->values = new_expr_list(6);
        CHECK_NULL(me->values);
    }

    while (!IS_TOKEN(tk, TK_RBR)) {
        key = parse_expr(p, P_LOWEST);
        CHECK_NULL(key);

        advance_token(p);
        tk = get_current_token(p);
        EXPECT_TOKEN(tk, TK_COLON);

        advance_token(p);
        value = parse_expr(p, P_LOWEST);

        if (expr_list_add(me->keys, key) < 0 ||
                expr_list_add(me->values, value) < 0) {
            return NULL;
        }

        advance_token(p);
        tk = get_current_token(p);
        if (!IS_TOKEN(tk, TK_RBR)) {
            EXPECT_TOKEN(tk, TK_COMMA);
            advance_token(p);
            tk = get_current_token(p);
        }
    }
    EXPECT_TOKEN(tk, TK_RBR);
    me->rbr = tk;

    return (struct expr *) me;
}
