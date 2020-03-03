#ifndef _M_M_H__
#define _M_M_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "util.h"
#include "hash.h"
#include "token.h"

enum node_type {
    N_INVALID_LWM, // low watermark
    N_STMT_VAR,
    N_STMT_RET,
    N_STMT_BLOCK,
    N_STMT_IF,
    N_STMT_FOR,
    N_STMT_EXPR,
    N_EXPR_IDENT,
    N_EXPR_LITERAL,
    N_EXPR_ARRAY,
    N_EXPR_MAP,
    N_EXPR_UNARY,
    N_EXPR_BINARY,
    N_EXPR_GROUP,
    N_EXPR_FUNC,
    N_EXPR_CALL,
    N_EXPR_INDEX,
    N_INVALID_HWM, // high watermark
};

const char *get_node_name(enum node_type type);

#define COBJECT           \
    enum node_type ntype; \
    unsigned int flag;    \
    struct token *tk;

#define SET_NODE_TYPE(n, nt)                                \
    do {                                                    \
        if ((nt) > N_INVALID_LWM && (nt) < N_INVALID_HWM) { \
            (n)->ntype = (nt);                              \
        } else {                                            \
            printf("set invalid node type: %d\n", (nt));    \
        }                                                   \
    } while (0)

#define NODE_TYPE(n) (n)->ntype
#define IS_NODE_TYPE(n, typ) ((n)->ntype == typ)

#include "lex.h"
#include "prec.h"
#include "expr.h"
#include "stmt.h"
#include "parser.h"

#include "iobject.h"
#include "treewalk_interpreter.h"
#include "builtin.h"

#endif
