#include "../include/m.h"

static struct tw_interp *new_tw_interp();

// start statements evaluation helper
static int tw_eval_if_stmt(struct tw_interp *interp, struct if_stmt *ist);
static int tw_eval_for_stmt(struct tw_interp *interp, struct for_stmt *fst);
static int tw_eval_block_stmt(struct tw_interp *interp, struct block_stmt *bst);
// end statements evaluation helper

// start expressions evalution helper
static struct iobject *tw_eval_ident_expr(struct tw_interp *interp, struct ident_expr *ie);

static struct iobject *tw_eval_literal_expr(struct tw_interp *interp, struct literal_expr *le);

static struct iobject *tw_eval_unary_expr(struct tw_interp *interp, struct unary_expr *ue);

static struct iobject *tw_eval_binary_expr(struct tw_interp *interp, struct binary_expr *be);
static struct iobject *tw_eval_int_binary_expr(struct tw_interp *interp, struct iobject *l, enum token_type tt,  struct iobject *r);
static struct iobject *tw_eval_number_binary_expr(struct tw_interp *interp, struct iobject *l, enum token_type tt, struct iobject *r);

static struct iobject *tw_eval_group_expr(struct tw_interp *interp, struct group_expr *ge);

static struct iobject *tw_eval_call_expr(struct tw_interp *interp, struct call_expr *ce);

static struct iobject *tw_eval_index_expr(struct tw_interp *interp, struct index_expr *ie);

static struct iobject *tw_eval_array_expr(struct tw_interp *interp, struct array_expr *ae);

static struct iobject *tw_eval_map_expr(struct tw_interp *interp, struct map_expr *me);

static struct iobject *tw_eval_func_expr(struct tw_interp *interp, struct func_expr *fe);

// end expressions evalution helper

int interp_init() {
    ENV_SET_TYPE(&__env_, ET_GLOBAL);
    __env_.parent = NULL;
    __env_.map = new_imap();
    CHECK_NULL_ERROR(__env_.map);

    __interp_ = new_tw_interp();
    CHECK_NULL_ERROR(__interp_);

    return ibuiltin_init();
}

///////// env object /////////////
struct tw_env *new_tw_env(struct tw_env *parent) {
    struct tw_env *env;

    env = (struct tw_env *) malloc (sizeof (struct tw_env));
    CHECK_NULL(env);
    ENV_SET_TYPE(&__env_, ET_INVALID_LWM);
    env->map = new_imap();
    env->parent = parent;

    return env;
}

struct iobject *tw_env_get(struct tw_env *env, struct iobject *key) {
    struct iobject *obj;

    obj = NULL;
    while (env && obj == NULL) {
        obj = imap_get(env->map, key);
        env = env->parent;
    }

    return obj;
}

int tw_env_clean(struct tw_env *env) {
    // TODO: do house-cleaning work
    RETURN_OK;
}

static struct tw_interp *new_tw_interp() {
    struct tw_interp *interp;

    interp = (struct tw_interp *) malloc (sizeof (struct tw_interp));
    CHECK_NULL(interp);
    interp->stk = new_ilist();
    CHECK_NULL(interp->stk);
    interp->env = GLOBAL_ENV();
    interp->consts = new_imap();
    CHECK_NULL(interp->consts);

    return interp;
}

int tw_empty(struct tw_interp *interp) {
    return interp->stk->size == 0;
}

int tw_push(struct tw_interp *interp, struct iobject *obj) {
    return ilist_add(interp->stk, obj);
}

struct iobject *tw_top(struct tw_interp *interp) {
    if (!tw_empty(interp)) {
        return ilist_get(interp->stk, interp->stk->size - 1);
    }
    return NULL;
}

void tw_pop(struct tw_interp *interp) {
    if (interp->stk->size <= 0) {
        printf("tree walk interpreter try to pop an empty stack\n");
        exit(-1);
    }
    interp->stk->size--;
}

int tw_eval(struct tw_interp *interp, struct program *prog) {
    int i, rc;
    rc = OK;
    if (!prog || !prog->stmts) {
        return rc;
    }

    for (i = 0; i < prog->stmts->size; i++) {
        if ((rc = tw_eval_stmt(interp, stmt_list_get(prog->stmts, i))) < 0) {
            return rc;
        }
    }

    return rc;
}

int tw_eval_stmt(struct tw_interp *interp, struct stmt *st) {
    int i, rc;
    struct iobject *obj;
    struct iliteral_object *literal;
    struct var_stmt *vst;
    struct return_stmt *rst;
    struct if_stmt *ist;
    struct for_stmt *fst;
    struct expr_stmt *est;

    rc = OK;

    switch (NODE_TYPE(st)) {
        case N_STMT_VAR:
            vst = (struct var_stmt *) st;
            literal = new_string_literal(vst->ident->literal);
            obj = tw_eval_expr(interp, vst->value);
            obj = VALUE(obj);
            CHECK_NULL_ERROR(obj);
            /*
            printf("$$ var %s = ", literal->val.s);
            iobject_print(obj);
            */
            rc = ENV_SET(interp->env, (struct iobject *) literal, obj);
            break;
        case N_STMT_RET:
            // ASSERT_ENV_TYPE(interp->env, ET_FUNC);
            rst = (struct return_stmt *) st;
            obj = tw_eval_expr(interp, rst->value);
            obj = VALUE(obj);
            CHECK_NULL_ERROR(obj);
            rc = tw_push(interp, obj);
            break;
        case N_STMT_IF:
            ist = (struct if_stmt *) st;
            rc = tw_eval_if_stmt(interp, ist);
            break;
        case N_STMT_FOR:
            fst = (struct for_stmt *) st;
            rc = tw_eval_for_stmt(interp, fst);
            break;
        case N_STMT_EXPR:
            est = (struct expr_stmt *) st;
            obj = tw_eval_expr(interp, est->e);
            obj = VALUE(obj);
            // printf("$$ ");
            // iobject_print(obj);
            break;
        default:
            break;
    }

    return rc;
}

static int tw_eval_if_stmt(struct tw_interp *interp, struct if_stmt *ist) {
    int i, rc, elif_eval_status;
    struct iobject *obj;
    struct block_stmt *bst;
    struct if_stmt *elif_st;
    rc = OK;

    obj = tw_eval_expr(interp, ist->cond);
    obj = VALUE(obj);
    CHECK_NULL_ERROR(obj);

    if (IS_TRUE(obj)) {
        bst = (struct block_stmt *) ist->then;
        rc = tw_eval_block_stmt(interp, bst);
    } else {
        elif_eval_status = 0;
        if (ist->elif_stmts) {
            for (i = 0; i < ist->elif_stmts->size; i++) {
                elif_st = (struct if_stmt *) stmt_list_get(ist->elif_stmts, i);
                obj = tw_eval_expr(interp, elif_st->cond);
                obj = VALUE(obj);
                CHECK_NULL_ERROR(obj);

                if (IS_TRUE(obj)) {
                    elif_eval_status = 1;
                    rc = tw_eval_block_stmt(interp, elif_st->then);
                    break;
                }
            }
        }

        if (!elif_eval_status && ist->_else) {
            rc = tw_eval_block_stmt(interp, ist->_else);
        }
    }

    return rc;
}

static int tw_eval_for_stmt(struct tw_interp *interp, struct for_stmt *fst) {
    int i, rc;
    struct tw_env *env;
    struct iobject *obj;
    rc = OK;

    ENTER_ENV(interp, env, ET_FOR)

    // initialization
    for (i = 0; fst->init && i < fst->init->size; i++) {
        tw_eval_expr(interp, expr_list_get(fst->init, i));
    }

    for (;;) {
        obj = tw_eval_expr(interp, fst->cond);
        obj = VALUE(obj);
        CHECK_NULL_ERROR(obj);
        if (IS_FALSE(obj)) {
            break;
        }

        rc = tw_eval_block_stmt(interp, fst->stmts);
        if (rc < 0) {
            break;
        }

        // post update
        for (i = 0; fst->post && i < fst->post->size; i++) {
            tw_eval_expr(interp, expr_list_get(fst->post, i));
        }
    }

    EXIT_ENV(interp, env)

    return rc;
}

static int tw_eval_block_stmt(struct tw_interp *interp, struct block_stmt *bst) {
    int i, rc;
    struct tw_env *env;
    rc = OK;

    ENTER_ENV(interp, env, ET_BLOCK)

    for (i = 0; i < bst->stmts->size; i++) {
        rc = tw_eval_stmt(interp, stmt_list_get(bst->stmts, i));
        if (rc < 0) {
            break;
        }
    }

    EXIT_ENV(interp, env)

    return rc;
}

struct iobject *tw_eval_expr(struct tw_interp *interp, struct expr *e) {
    int i;
    struct iobject *obj;

    obj = NULL;
    switch (NODE_TYPE(e)) {
        case N_EXPR_IDENT:
            obj = tw_eval_ident_expr(interp, (struct ident_expr *) e);
            break;
        case N_EXPR_LITERAL:
            obj = tw_eval_literal_expr(interp, (struct literal_expr *) e);
            break;
        case N_EXPR_UNARY:
            obj = tw_eval_unary_expr(interp, (struct unary_expr *) e);
            break;
        case N_EXPR_BINARY:
            obj = tw_eval_binary_expr(interp, (struct binary_expr *) e);
            break;
        case N_EXPR_FUNC:
            obj = tw_eval_func_expr(interp, (struct func_expr *) e);
            break;
        case N_EXPR_GROUP:
            obj = tw_eval_group_expr(interp, (struct group_expr *) e);
            break;
        case N_EXPR_CALL:
            obj = tw_eval_call_expr(interp, (struct call_expr *) e);
            break;
        case N_EXPR_INDEX:
            obj = tw_eval_index_expr(interp, (struct index_expr *) e);
            break;
        case N_EXPR_ARRAY:
            obj = tw_eval_array_expr(interp, (struct array_expr *) e);
            break;
        case N_EXPR_MAP:
            obj = tw_eval_map_expr(interp, (struct map_expr *) e);
            break;
        default:
            break;
    }

    return obj;
}

static struct iobject *tw_eval_ident_expr(struct tw_interp *interp, struct ident_expr *ie) {
    struct iobject *obj;
    struct iliteral_object *literal;

    obj = NULL;
    literal = find_string_literal(interp->consts, ie->tk->literal);
    if (literal) {
        obj = ENV_GET(interp->env, (struct iobject *) literal);
    }
    return obj;
}

static struct iobject *tw_eval_literal_expr(struct tw_interp *interp, struct literal_expr *le) {
    struct iliteral_object *obj;

    obj = NULL;
    switch (le->tk->type) {
        case TK_NIL:
            RETURN_NIL;
        case TK_TRUE:
            RETURN_TRUE;
        case TK_FALSE:
            RETURN_FALSE;
        case TK_CHAR:
            obj = find_char_literal(interp->consts, le->value.c);
            break;
        case TK_INT:
            obj = find_int_literal(interp->consts, le->value.l);
            break;
        case TK_FLOAT:
            obj = find_float_literal(interp->consts, le->value.f);
            break;
        case TK_STRING:
            obj = find_string_literal(interp->consts, le->value.s);
            break;
        default:
            printf("Error: line %d column %d unkown literal type to eval, %s\n",
                    le->tk->line,
                    le->tk->col,
                    token_expr(le->tk->type));
            break;
    }

    return (struct iobject *) obj;
}

static struct iobject *tw_eval_unary_expr(struct tw_interp *interp, struct unary_expr *ue) {
    struct iobject *obj, *v;
    struct iliteral_object *literal;

    obj = tw_eval_expr(interp, ue->e);
    v = VALUE(obj);
    CHECK_NULL(v);
    switch (ue->tk->type) {
        case TK_NEGATE:
            if (!IS_IOBJECT_TYPE(v, OT_INT)) {
                printf("prefix \"~\" operator can only be used on type INT\n");
                exit(-1);
            }
            literal = (struct iliteral_object *) v;
            literal->val.l = ~literal->val.l;
            obj = v;
            break;
        case TK_NOT:
            if (!IS_IOBJECT_TYPE(v, OT_BOOL)) {
                printf("prefix \"!\" operator can only be used on type BOOL\n");
                exit(-1);
            }

            if (IS_TRUE(v)) {
                RETURN_FALSE;
            } else {
                RETURN_TRUE;
            }
            break;
        case TK_MINUS:
            if (IS_IOBJECT_TYPE(v, OT_INT)) {
                literal = (struct iliteral_object *) v;
                literal->val.l = -literal->val.l;
            } else if (IS_IOBJECT_TYPE(v, OT_FLOAT)) {
                literal = (struct iliteral_object *) v;
                literal->val.f = -literal->val.f;
            } else {
                printf("prefix \"-\" operator can only be used on type INT or FLOAT\n");
                exit(-1);
            }
            obj = v;
            break;
        case TK_PLUS:
        default:
            break;
    }

    return obj;
}

static struct iobject *tw_eval_int_binary_expr(struct tw_interp *interp, struct iobject *l, enum token_type tt, struct iobject *r) {
    struct iobject *v, *res;
    long il, ir;

    CHECK_NULL(l);
    CHECK_NULL(r);

    v = VALUE(l);
    if (!IS_IOBJECT_TYPE(v, OT_INT)) {
        return NULL;
    }
    il = ((struct iliteral_object *) v)->val.l;

    v = VALUE(r);
    if (!IS_IOBJECT_TYPE(v, OT_INT)) {
        return NULL;
    }
    ir = ((struct iliteral_object *) v)->val.l;

    switch (tt) {
        case TK_PERCENT:
            if (ir == 0) {
                return NULL;
            }
            res = (struct iobject *) find_int_literal(interp->consts, il % ir);
            break;
        case TK_AND:
            res = (struct iobject *) find_int_literal(interp->consts, il & ir);
            break;
        case TK_OR:
            res = (struct iobject *) find_int_literal(interp->consts, il | ir);
            break;
        case TK_XOR:
            res = (struct iobject *) find_int_literal(interp->consts, il ^ ir);
            break;
        case TK_LSHIFT:
            if (ir < 0) {
                return NULL;
            }
            res = (struct iobject *) find_int_literal(interp->consts, il << ir);
            break;
        case TK_RSHIFT:
            if (ir < 0) {
                return NULL;
            }
            res = (struct iobject *) find_int_literal(interp->consts, il >> ir);
            break;
        case TK_PERCENT_ASSIGN:
            if (ir == 0 || !ADDRESSABLE(l)) {
                return NULL;
            }
            res = (struct iobject *) new_int_literal(il % ir);
            SET_ADDR_VALUE(l, res);
            res = l;
            break;
        case TK_AND_ASSIGN:
            if (!ADDRESSABLE(l)) {
                return NULL;
            }
            res = (struct iobject *) new_int_literal(il & ir);
            SET_ADDR_VALUE(l, res);
            res = l;
            break;
        case TK_OR_ASSIGN:
            if (!ADDRESSABLE(l)) {
                return NULL;
            }
            res = (struct iobject *) new_int_literal(il | ir);
            SET_ADDR_VALUE(l, res);
            res = l;
            break;
        case TK_XOR_ASSIGN:
            if (!ADDRESSABLE(l)) {
                return NULL;
            }
            res = (struct iobject *) new_int_literal(il ^ ir);
            SET_ADDR_VALUE(l, res);
            res = l;
            break;
        case TK_LSHIFT_ASSIGN:
            if (ir < 0 || !ADDRESSABLE(l)) {
                return NULL;
            }
            res = (struct iobject *) new_int_literal(il << ir);
            SET_ADDR_VALUE(l, res);
            res = l;
            break;
        case TK_RSHIFT_ASSIGN:
            if (ir < 0 || !ADDRESSABLE(l)) {
                return NULL;
            }
            res = (struct iobject *) new_int_literal(il >> ir);
            SET_ADDR_VALUE(l, res);
            res = l;
            break;
        default:
            break;
    }

    return res;
}

static struct iobject *tw_eval_number_binary_expr(struct tw_interp *interp, struct iobject *l, enum token_type tt, struct iobject *r) {
    struct iobject *v, *res;
    int kind; // 1: (int, int), 2: (int, float), 3: (float, int), 4: (float, float)
    long il, ir;
    double fl, fr;

    CHECK_NULL(l);
    CHECK_NULL(r);

    kind = 0;
    v = VALUE(l);
    if (IS_IOBJECT_TYPE(v, OT_INT)) {
        il = ((struct iliteral_object *) v)->val.l;

        v = VALUE(r);
        if (IS_IOBJECT_TYPE(v, OT_INT)) {
            ir = ((struct iliteral_object *) v)->val.l;
            kind = 1;
        } else if (IS_IOBJECT_TYPE(v, OT_FLOAT)) {
            fr = ((struct iliteral_object *) v)->val.l;
            kind = 2;
        }
    } else if (IS_IOBJECT_TYPE(v, OT_FLOAT)) {
        fl = ((struct iliteral_object *) v)->val.f;

        v = VALUE(r);
        if (IS_IOBJECT_TYPE(v, OT_INT)) {
            ir = ((struct iliteral_object *) v)->val.l;
            kind = 3;
        } else if (IS_IOBJECT_TYPE(v, OT_FLOAT)) {
            fr = ((struct iliteral_object *) v)->val.l;
            kind = 4;
        }
    }

    if (kind == 0) {
        return NULL;
    }

    switch (tt) {
        case TK_PLUS:
            if (kind == 1) {
                res = (struct iobject *) find_int_literal(interp->consts, il + ir);
            } else if (kind == 2) {
                res = (struct iobject *) find_float_literal(interp->consts, ((double) il) + fr);
            } else if (kind == 3) {
                res = (struct iobject *) find_float_literal(interp->consts, fl + ((double) ir));
            } else if (kind == 4) {
                res = (struct iobject *) find_float_literal(interp->consts, fl + fr);
            }
            break;
        case TK_MINUS:
            if (kind == 1) {
                res = (struct iobject *) find_int_literal(interp->consts, il - ir);
            } else if (kind == 2) {
                res = (struct iobject *) find_float_literal(interp->consts, ((double) il) - fr);
            } else if (kind == 3) {
                res = (struct iobject *) find_float_literal(interp->consts, fl - ((double) ir));
            } else if (kind == 4) {
                res = (struct iobject *) find_float_literal(interp->consts, fl - fr);
            }
            break;
        case TK_STAR:
            if (kind == 1) {
                res = (struct iobject *) find_int_literal(interp->consts, il * ir);
            } else if (kind == 2) {
                res = (struct iobject *) find_float_literal(interp->consts, ((double) il) * fr);
            } else if (kind == 3) {
                res = (struct iobject *) find_float_literal(interp->consts, fl * ((double) ir));
            } else if (kind == 4) {
                res = (struct iobject *) find_float_literal(interp->consts, fl * fr);
            }
            break;
        case TK_SLASH:
            if (kind == 1) {
                if (ir == 0) {
                    res = NULL;
                } else {
                    res = (struct iobject *) find_int_literal(interp->consts, il / ir);
                }
            } else if (kind == 2) {
                if (fr == 0.0) {
                    res = NULL;
                } else {
                    res = (struct iobject *) find_float_literal(interp->consts, ((double) il) / fr);
                }
            } else if (kind == 3) {
                if (ir == 0) {
                    res = NULL;
                } else {
                    res = (struct iobject *) find_float_literal(interp->consts, fl / ((double) ir));
                }
            } else if (kind == 4) {
                if (fr == 0.0) {
                    res = NULL;
                } else {
                    res = (struct iobject *) find_float_literal(interp->consts, fl / fr);
                }
            }
            break;
        case TK_PLUS_ASSIGN:
            if (!ADDRESSABLE(l)) {
                return NULL;
            }

            if (kind == 1) {
                res = (struct iobject *) new_int_literal(il + ir);
            } else if (kind == 2) {
                res = (struct iobject *) new_float_literal(((double) il) + fr);
            } else if (kind == 3) {
                res = (struct iobject *) new_float_literal(fl + ((double) ir));
            } else if (kind == 4) {
                res = (struct iobject *) new_float_literal(fl + fr);
            }

            if (res != NULL) {
                SET_ADDR_VALUE(l, res);
                res = l;
            }
            break;
        case TK_MINUS_ASSIGN:
            if (!ADDRESSABLE(l)) {
                return NULL;
            }

            if (kind == 1) {
                res = (struct iobject *) new_int_literal(il - ir);
            } else if (kind == 2) {
                res = (struct iobject *) new_float_literal(((double) il) - fr);
            } else if (kind == 3) {
                res = (struct iobject *) new_float_literal(fl - ((double) ir));
            } else if (kind == 4) {
                res = (struct iobject *) new_float_literal(fl - fr);
            }

            if (res != NULL) {
                SET_ADDR_VALUE(l, res);
                res = l;
            }
            break;
        case TK_STAR_ASSIGN:
            if (!ADDRESSABLE(l)) {
                return NULL;
            }

            if (kind == 1) {
                res = (struct iobject *) new_int_literal(il * ir);
            } else if (kind == 2) {
                res = (struct iobject *) new_float_literal(((double) il) * fr);
            } else if (kind == 3) {
                res = (struct iobject *) new_float_literal(fl * ((double) ir));
            } else if (kind == 4) {
                res = (struct iobject *) new_float_literal(fl * fr);
            }

            if (res != NULL) {
                SET_ADDR_VALUE(l, res);
                res = l;
            }
            break;
        case TK_SLASH_ASSIGN:
            if (!ADDRESSABLE(l)) {
                return NULL;
            }

            if (kind == 1) {
                if (ir == 0) {
                    res = NULL;
                } else {
                    res = (struct iobject *) new_int_literal(il / ir);
                }
            } else if (kind == 2) {
                if (fr == 0.0) {
                    res = NULL;
                } else {
                    res = (struct iobject *) new_float_literal(((double) il) / fr);
                }
            } else if (kind == 3) {
                if (ir == 0) {
                    res = NULL;
                } else {
                    res = (struct iobject *) new_float_literal(fl / ((double) ir));
                }
            } else if (kind == 4) {
                if (fr == 0.0) {
                    res = NULL;
                } else {
                    res = (struct iobject *) new_float_literal(fl / fr);
                }
            }

            if (res != NULL) {
                SET_ADDR_VALUE(l, res);
                res = l;
            }
            break;
        default:
            break;
    }

    return res;
}

static struct iobject *tw_eval_binary_expr(struct tw_interp *interp, struct binary_expr *be) {
    struct iobject *lobj, *robj, *res;

    lobj = tw_eval_expr(interp, be->left);
    CHECK_NULL(lobj);
    robj = tw_eval_expr(interp, be->right);
    CHECK_NULL(robj);

    switch (be->op->type) {
        case TK_ASSIGN:
            if (!ADDRESSABLE(lobj)) {
                exit(-1);
            }
            robj = VALUE(robj);
            SET_ADDR_VALUE(lobj, robj);
            res = lobj;
            break;
        case TK_PLUS:
        case TK_MINUS:
        case TK_STAR:
        case TK_SLASH:
        case TK_PLUS_ASSIGN:
        case TK_MINUS_ASSIGN:
        case TK_STAR_ASSIGN:
        case TK_SLASH_ASSIGN:
            res = tw_eval_number_binary_expr(interp, lobj, be->op->type, robj);
            break;
        case TK_PERCENT:
        case TK_AND:
        case TK_OR:
        case TK_XOR:
        case TK_LSHIFT:
        case TK_RSHIFT:
        case TK_PERCENT_ASSIGN:
        case TK_AND_ASSIGN:
        case TK_OR_ASSIGN:
        case TK_XOR_ASSIGN:
        case TK_LSHIFT_ASSIGN:
        case TK_RSHIFT_ASSIGN:
            res = tw_eval_int_binary_expr(interp, lobj, be->op->type, robj);
            break;
        case TK_LAND:
            res = VALUE(lobj);
            if (!IS_IOBJECT_TYPE(res, OT_BOOL)) {
                return NULL;
            }

            if (IS_FALSE(res)) {
                RETURN_FALSE;
            }

            res = VALUE(robj);
            if (!IS_IOBJECT_TYPE(res, OT_BOOL)) {
                return NULL;
            }
            break;
        case TK_LOR:
            res = VALUE(lobj);
            if (!IS_IOBJECT_TYPE(res, OT_BOOL)) {
                return NULL;
            }

            if (IS_TRUE(res)) {
                RETURN_TRUE;
            }

            res = VALUE(robj);
            if (!IS_IOBJECT_TYPE(res, OT_BOOL)) {
                return NULL;
            }
            break;
        case TK_EQ:
            if (iobject_equals(VALUE(lobj), VALUE(robj))) {
                RETURN_TRUE;
            }
            RETURN_FALSE;
        case TK_NE:
            if (!iobject_equals(VALUE(lobj), VALUE(robj))) {
                RETURN_TRUE;
            }
            RETURN_FALSE;
        case TK_LT:
            if (iobject_less_than(VALUE(lobj), VALUE(robj))) {
                RETURN_TRUE;
            }
            RETURN_FALSE;
        case TK_LE:
            lobj = VALUE(lobj);
            robj = VALUE(robj);
            if (iobject_less_than(lobj, robj) || iobject_equals(lobj, robj)) {
                RETURN_TRUE;
            }
            RETURN_FALSE;
        case TK_GT:
            if (!iobject_less_than(VALUE(lobj), VALUE(robj))) {
                RETURN_TRUE;
            }
            RETURN_FALSE;
        case TK_GE:
            lobj = VALUE(lobj);
            robj = VALUE(robj);
            if (!iobject_less_than(lobj, robj) || iobject_equals(lobj, robj)) {
                RETURN_TRUE;
            }
            RETURN_FALSE;
        default:
            break;
    }

    return res;
}

static struct iobject *tw_eval_group_expr(struct tw_interp *interp, struct group_expr *ge) {
    struct iobject *obj;
    obj = tw_eval_expr(interp, ge->e);
    obj = VALUE(obj);
    return obj;
}

static struct iobject *tw_eval_call_expr(struct tw_interp *interp, struct call_expr *ce) {
    int i, n;
    struct token_list *tparams;
    struct block_stmt *bst;
    struct iobject *obj, *eobj;
    struct iliteral_object *literal;
    struct ifunc_object *fobj;
    struct ilist_object *eparams;
    struct ibuiltin_func *bltin_fn;
    struct tw_env *env;

    obj = tw_eval_expr(interp, ce->fn);
    CHECK_NULL(obj);
    obj = VALUE(obj);

    n = 0;
    if (ce->params) {
        n = ce->params->size;
    }
    eparams = new_ilist();
    for (i = 0; i < n; i++) {
        eobj = tw_eval_expr(interp, expr_list_get(ce->params, i));
        if (ilist_add(eparams, eobj) < 0) {
            return NULL;
        }
    }

    if (IS_IOBJECT_TYPE(obj, OT_BUILTIN_FUNC)) {
        bltin_fn = (struct ibuiltin_func *) obj;
        return bltin_fn->fn(interp, (struct iobject *) eparams);
    }

    if (!IS_IOBJECT_TYPE(obj, OT_FUNC)) {
        return NULL;
    }
    fobj = (struct ifunc_object *) obj;

    tparams=fobj->fn->params;
    if ((!tparams && n) || (tparams && tparams->size != n)) {
        printf("# of params unmatched\n");
        return NULL;
    }

    ENTER_ENV(interp, env, ET_FUNC)

    if (interp->stk->size > 0) {
        return NULL;
    }

    for (i = 0; i < n; i++) {
        literal = find_string_literal(interp->consts, token_list_get(tparams, i)->literal);
        obj = ilist_get(eparams, i);
        ENV_SET(interp->env, (struct iobject *) literal,  obj);
    }

    obj = NULL;
    bst = fobj->fn->stmts;
    for (i = 0; i < bst->stmts->size; ++i) {
        if (tw_eval_stmt(interp, stmt_list_get(bst->stmts, i)) < 0) {
            printf("eval func stmt %d failed\n", i);
            return NULL;
        }

        if (!tw_empty(interp)) {
            obj = tw_top(interp);
            tw_pop(interp);
            break;
        }
    }

    EXIT_ENV(interp, env)

    if (obj == NULL) {
        SET_NIL(obj);
    }

    return obj;
}

static struct iobject *tw_eval_index_expr(struct tw_interp *interp, struct index_expr *ie) {
    int i, j;
    struct iobject *obj, *v, *s, *e;
    struct ilist_object *list;

    v = tw_eval_expr(interp, ie->e);
    v = VALUE(v);
    CHECK_NULL(v);

    obj = s = e = NULL;
    i = j = -1;
    if (ie->start) {
        s = tw_eval_expr(interp, ie->start);
        CHECK_NULL(s);
        s = VALUE(s);
        if (IS_IOBJECT_TYPE(s, OT_INT)) {
            i = ((struct iliteral_object *) s)->val.l;
        }
    }

    if (ie->end) {
        e = tw_eval_expr(interp, ie->end);
        CHECK_NULL(e);
        e = VALUE(e);
        if (!IS_IOBJECT_TYPE(e, OT_INT)) {
            return NULL;
        }
        j = ((struct iliteral_object *) e)->val.l;
    } else if (ie->colon) {
        if (!IS_IOBJECT_TYPE(v, OT_ARRAY)) {
            return NULL;
        }
        j =  ((struct ilist_object *) v)->size;
    }

    if (IS_IOBJECT_TYPE(v, OT_ARRAY)) {
        list = (struct ilist_object *) v;
        if (i >= 0) {
            if (j >= 0) {
                obj = (struct iobject *) ilist_get_range(list, i, j);
            } else {
                obj = ilist_get(list, i);
            }
        } else {
            if (j >= 0) {
                obj = (struct iobject *) ilist_get_range(list, 0, j);
            }
        }
    } else if (IS_IOBJECT_TYPE(v, OT_MAP)) {
        CHECK_NULL(s);
        obj = imap_get_or_create((struct imap_object *) v, s);
    }

    return obj;
}

static struct iobject *tw_eval_array_expr(struct tw_interp *interp, struct array_expr *ae) {
    int i, n;
    struct ilist_object *list;
    struct iobject *obj;

    list = new_ilist();
    CHECK_NULL(list);

    i = n = 0;
    if (ae->elems) {
        n = ae->elems->size;
    }

    for (; i < n; i++) {
        obj = tw_eval_expr(interp, expr_list_get(ae->elems, i));
        obj = VALUE(obj);
        obj = (struct iobject *) new_imap_item(obj, obj);
        obj = new_iaddress(obj);
        if (ilist_add(list, obj) < 0) {
            break;
        }
    }

    return (struct iobject *) list;
}

static struct iobject *tw_eval_map_expr(struct tw_interp *interp, struct map_expr *me) {
    int i, n;
    struct imap_object *map;
    struct iobject *k, *v;

    map = new_imap();
    CHECK_NULL(map);

    if (me->keys && me->values) {
        n = me->keys->size;
        if (me->values->size != n) {
            n = 0;
        }
    }

    for (i = 0; i < n; i++) {
        k = tw_eval_expr(interp, expr_list_get(me->keys, i));
        k = VALUE(k);
        v = tw_eval_expr(interp, expr_list_get(me->values, i));
        v = VALUE(v);
        if (imap_add(map, k, v) < 0) {
            exit(-1);
        }
    }

    return (struct iobject *) map;
}

static struct iobject *tw_eval_func_expr(struct tw_interp *interp, struct func_expr *fe) {
    return (struct iobject *) new_ifunc_object(fe);
}
