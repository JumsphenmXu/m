#include "../include/m.h"

struct block_stmt *parse_block_stmt(struct parser *p, struct token *tk) {
    struct block_stmt *bst;
    struct stmt *st;
    int rc;

    EXPECT_TOKEN(tk, TK_LBR);
    bst = (struct block_stmt *) malloc (sizeof (struct block_stmt));
    CHECK_NULL(bst);
    SET_NODE_TYPE(bst, N_STMT_BLOCK);
    bst->stmts = new_stmt_list(0);
    CHECK_NULL(bst->stmts);
    bst->tk = tk;
    advance_token(p);

    rc = 0;
    for (;;) {
        tk = get_current_token(p);
        switch (tk->type) {
            case TK_INVALID:
            case TK_EOF:
                rc = -1;
                break;
            case TK_SEMICOLON:
                advance_token(p);
                break;
            case TK_RBR:
                bst->rbr = tk;
                return bst;
            default:
                if ((st = (struct stmt *) parse_stmt(p, tk))) {
                    rc = stmt_list_add(bst->stmts, st);
                }
                advance_token(p);
                break;
        }

        if (rc < 0) {
            break;
        }
    }

    return bst;
}
