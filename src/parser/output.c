#include "../include/m.h"

#define NTRANS(i, n)  \
    while (i < (n)) { \
        printf("\t"); \
        i++;          \
    }

static void print_var_stmt(struct var_stmt *st, int n);
static void print_return_stmt(struct return_stmt *st, int n);
static void print_block_stmt(struct block_stmt *st, int n);
static void print_if_stmt(struct if_stmt *st, int n);
static void print_for_stmt(struct for_stmt *st, int n);
static void print_expr_stmt(struct expr_stmt *st, int n);
static void print_expr(struct expr *e);
static void print_ident_expr(struct ident_expr *e);
static void print_literal_expr(struct literal_expr *e);
static void print_array_expr(struct array_expr *e);
static void print_map_expr(struct map_expr *e);
static void print_unary_expr(struct unary_expr *e);
static void print_binary_expr(struct binary_expr *e);
static void print_group_expr(struct group_expr *e);
static void print_func_expr(struct func_expr *e);
static void print_call_expr(struct call_expr *e);
static void print_index_expr(struct index_expr *e);

void print_stmt(struct stmt *st, int n) {
    if (st == NULL) {
        printf("stmt is NULL");
        return;
    }

    switch (NODE_TYPE(st)) {
        case N_STMT_VAR:
            print_var_stmt((struct var_stmt *) st, n);
            break;
        case N_STMT_RET:
            print_return_stmt((struct return_stmt *) st, n);
            break;
        case N_STMT_BLOCK:
            print_block_stmt((struct block_stmt *) st, n);
            break;
        case N_STMT_IF:
            print_if_stmt((struct if_stmt *) st, n);
            break;
        case N_STMT_FOR:
            print_for_stmt((struct for_stmt *) st, n);
            break;
        case N_STMT_EXPR:
            print_expr_stmt((struct expr_stmt *) st, n);
            break;
        default:
            printf("unknown stmt type\n");
            break;
    }
}

static void print_var_stmt(struct var_stmt *st, int n) {
    int i;
    NTRANS(i, n);
    printf("%s", token_expr(st->tk->type));
    ASSERT_NOT_NULL(st->ident);
    ASSERT_NOT_NULL(st->ident->literal);
    printf(" %s ", st->ident->literal);
    ASSERT_NOT_NULL(st->value);
    print_expr(st->value);
    printf("\n");
}

static void print_return_stmt(struct return_stmt *st, int n) {
    int i;
    NTRANS(i, n);
    printf("%s ", token_expr(st->tk->type));
    print_expr(st->value);
    printf("\n");
}

static void print_block_stmt(struct block_stmt *st, int n) {
    int i, j;
    NTRANS(i, n);
    printf("{\n");
    for (j = 0; st->stmts && j < st->stmts->size; ++j) {
        NTRANS(i, n + 1);
        print_stmt(stmt_list_get(st->stmts, j), n + 1);
    }
    NTRANS(i, n);
    printf("}\n");
}

static void print_if_stmt(struct if_stmt *st, int n) {
    int i, j;
    struct if_stmt *ist;
    NTRANS(i, n);
    printf("%s (", token_expr(st->tk->type));
    print_expr(st->cond);
    printf(")");
    print_block_stmt(st->then, n);
    if (st->elif_stmts) {
        for (j = 0; j < st->elif_stmts->size; ++j) {
            ist = (struct if_stmt *) stmt_list_get(st->elif_stmts, j);
            printf("%s (", token_expr(ist->tk->type));
            print_expr(ist->cond);
            printf(")");
            print_block_stmt(ist->then, n);
        }
    }
    if (st->_else) {
        NTRANS(i, n);
        printf(" %s ", token_expr(st->_else->tk->type));
        print_block_stmt(st->_else, n);
    }
}

static void print_for_stmt(struct for_stmt *st, int n) {
    int i;
    NTRANS(i, n);
    printf("%s (", token_expr(st->tk->type));
    for (i = 0; st->init && i < st->init->size; i++) {
        print_expr(expr_list_get(st->init, i));
        if (i < st->init->size - 1) {
            printf(",");
        }
    }
    printf(";");
    if (st->cond) {
        print_expr(st->cond);
    }
    printf(";");
    for (i = 0; st->post && i < st->post->size; i++) {
        print_expr(expr_list_get(st->post, i));
        if (i < st->post->size - 1) {
            printf(",");
        }
    }
    printf(")");
    print_block_stmt(st->stmts, n);
    printf("\n");
}

static void print_expr_stmt(struct expr_stmt *st, int n) {
    int i;
    NTRANS(i, n);
    print_expr(st->e);
    printf("\n");
}

static void print_expr(struct expr *e) {
    enum node_type type = NODE_TYPE(e);
    // printf("%s", get_node_name(type));
    switch (type) {
        case N_EXPR_IDENT:
            print_ident_expr((struct ident_expr *) e);
            break;
        case N_EXPR_LITERAL:
            print_literal_expr((struct literal_expr *) e);
            break;
        case N_EXPR_ARRAY:
            print_array_expr((struct array_expr *) e);
            break;
        case N_EXPR_MAP:
            print_map_expr((struct map_expr *) e);
            break;
        case N_EXPR_UNARY:
            print_unary_expr((struct unary_expr *) e);
            break;
        case N_EXPR_BINARY:
            print_binary_expr((struct binary_expr *) e);
            break;
        case N_EXPR_GROUP:
            print_group_expr((struct group_expr *) e);
            break;
        case N_EXPR_FUNC:
            print_func_expr((struct func_expr *) e);
            break;
        case N_EXPR_CALL:
            print_call_expr((struct call_expr *) e);
            break;
        case N_EXPR_INDEX:
            print_index_expr((struct index_expr *) e);
            break;
        default:
            printf("unknown expression type %d, %s\n", type, get_token_info(e->tk));
            break;
    }
}

static void print_ident_expr(struct ident_expr *e) {
    ASSERT_NOT_NULL(e->tk);
    printf("%s", e->tk->literal);
}

static void print_literal_expr(struct literal_expr *e) {
    ASSERT_NOT_NULL(e->tk);
    switch (e->tk->type) {
        case TK_CHAR:
            printf("'%s'", e->tk->literal);
            break;
        case TK_INT:
        case TK_FLOAT:
        case TK_TRUE:
        case TK_FALSE:
            printf("%s", e->tk->literal);
            break;
        case TK_STRING:
            printf("\"%s\"", e->tk->literal);
            break;
        default:
            printf("unknown literal expression, %s\n", get_token_info(e->tk));
            break;
    }
}

static void print_unary_expr(struct unary_expr *e) {
    ASSERT_NOT_NULL(e->tk);
    switch (e->tk->type) {
        case TK_NEGATE:
        case TK_MINUS:
        case TK_NOT:
            printf("(");
            printf("%s", e->tk->literal ? e->tk->literal : "");
            print_expr(e->e);
            printf(")");
            break;
        default:
            print_expr(e->e);
            break;
    }
}

static void print_binary_expr(struct binary_expr *e) {
    printf("(");
    print_expr(e->left);
    printf(" %s ", e->op->literal);
    print_expr(e->right);
    printf(")");
}

static void print_group_expr(struct group_expr *e) {
    printf("(");
    print_expr(e->e);
    printf(")");
}

static void print_func_expr(struct func_expr *e) {
    int i, n;
    printf("%s(", token_expr(e->tk->type));
    if (e->params) {
        n = e->params->size;
        for (i = 0; i < n; i++) {
            printf("%s", token_list_get(e->params, i)->literal);
            if (i < n - 1) {
                printf(", ");
            }
        }
    }
    printf(")");
    print_block_stmt(e->stmts, 0);
}

static void print_call_expr(struct call_expr *e) {
    int i, n;
    printf("(");
    print_expr(e->fn);
    printf("(");
    if (e->params) {
        n = e->params->size;
        for (i = 0; i < n; i++) {
            print_expr(expr_list_get(e->params, i));
            if (i < n - 1) {
                printf(",");
            }
        }
    }
    printf(")");
    printf(")");
}

static void print_index_expr(struct index_expr *e) {
    printf("(");
    print_expr(e->e);
    printf("[");
    if (e->start) {
        print_expr(e->start);
    }
    if (e->colon) {
        printf(":");
    }
    if (e->end) {
        print_expr(e->end);
    }
    printf("]");
    printf(")");
}

static void print_array_expr(struct array_expr *e) {
    int i, n;
    printf("[");
    if (e->elems) {
        n = e->elems->size;
        for (i = 0; i < n; ++i) {
            print_expr(expr_list_get(e->elems, i));
            if (i < n - 1) {
                printf(",");
            }
        }
    }
    printf("]");
}

static void print_map_expr(struct map_expr *e) {
    int i, n;
    printf("{");
    if (e->keys && e->values) {
        n = e->keys->size;
        if (n != e->values->size) {
            printf("map: length of keys is not the same as length of values");
            printf("}");
            return;
        }

        for (i = 0; i < n; i++) {
            print_expr(expr_list_get(e->keys, i));
            printf(":");
            print_expr(expr_list_get(e->values, i));
            if (i < n - 1) {
                printf(", ");
            }
        }
    }
    printf("}");
}
