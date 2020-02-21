#ifndef _M_EXPR_H__
#define _M_EXPR_H__

#define F_IMMUTABLE (1 << 0)
#define IS_IMMUTABLE(e) ((e)->flag & F_IMMUTABLE)
#define SET_IMMUTABLE(e) ((e)->flag |= F_IMMUTABLE)
#define SET_MUTABLE(e) ((e)->flag &= ~F_IMMUTABLE)

#define INIT_EXPR_LIST_SIZE 6

struct parser;

enum prec get_precedence(struct token *tk);

struct expr {
    COBJECT
};
struct expr *parse_expr(struct parser *p, enum prec precedence);

struct expr_list {
    unsigned int size, cap;
    struct expr **exprs;
};
struct expr_list *new_expr_list(unsigned int init_cap);
struct expr *expr_list_get(struct expr_list *list, unsigned int i);
int expr_list_expand(struct expr_list *list);
int expr_list_add(struct expr_list *list, struct expr *e);
int expr_list_clean(struct expr_list *list);

// ident
struct ident_expr {
    COBJECT
};
struct expr *parse_ident_expr(struct parser *p);

// true, false, 1, 1.01, "char"
struct literal_expr {
    COBJECT
    union {
        long l;
        double f;
        char *s;
        int b;
    } value;
};
struct expr *parse_literal_expr(struct parser *p);

// unary_expr: [op] expr
struct unary_expr {
    COBJECT
    struct expr *e;
};
struct expr *parse_unary_expr(struct parser *p);

// binary_expr: expr op expr
struct binary_expr {
    COBJECT
    struct expr *left;
    struct token *op;
    struct expr *right;
};
struct expr *parse_binary_expr(struct parser *p, struct expr *left);

// '(' expr ')'
struct group_expr {
    COBJECT
    struct expr *e;
    struct token *rp;
};
struct expr *parse_group_expr(struct parser *p, struct token *tk);

// func: 'func' [ident] '(' parms ')' block_stmt
// params: token ',' token ',' token...
struct func_expr {
    COBJECT
    struct token *lp;
    struct token_list *params;
    struct token *rp;
    struct block_stmt *stmts;
};
struct expr *parse_func_expr(struct parser *p, struct token *tk);

// a[start ':' end], a[start], a[start ':']
struct index_expr {
    COBJECT
    struct expr *e;
    struct token *lsq;
    struct expr *start;
    struct token *colon;
    struct expr *end;
    struct token *rsq;
};
struct expr *parse_index_expr(struct parser *p, struct expr *e);

// ident '(' params ')'
// func '(' params ')' block_stmts '(' params ')'
struct call_expr {
    COBJECT
    struct expr *fn;
    struct token *lp;
    struct expr_list *params;
    struct token *rp;
};
struct expr *parse_call_expr(struct parser *p, struct expr *e);

// '[' expr ',' expr ',' expr ',' ... ']'
struct array_expr {
    COBJECT
    struct expr_list *elems;
    struct token *rsq;
};
struct expr *parse_array_expr(struct parser *p, struct token *tk);

// '{' expr: expr, expr: expr '}'
struct map_expr {
    COBJECT
    struct expr_list *keys;
    struct expr_list *values;
    struct token *rbr;
};
struct expr *parse_map_expr(struct parser *p, struct token *tk);
#endif
