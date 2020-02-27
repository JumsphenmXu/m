#ifndef _M_STMT_H__
#define _M_STMT_H__

struct parser;
struct expr;

// stmt_trailer: ';'
// stmt_trailer: '\n'
//
// stmt: expr_stmt
// stmt: var_stmt
// stmt: return_stmt
// stmt: if_stmt
// stmt: for_stmt
// stmt: block_stmt

struct stmt {
    COBJECT
};
struct stmt *new_stmt();
struct stmt *parse_stmt(struct parser *p, struct token *tk);
void print_stmt(struct stmt *st, int n);

#define INIT_STMT_LIST_SIZE 20

struct stmt_list {
    unsigned int size, cap;
    struct stmt **stmts;
};
struct stmt_list *new_stmt_list(unsigned int stmt_num);
struct stmt *stmt_list_get(struct stmt_list *list, int i);
int stmt_list_add(struct stmt_list *list, struct stmt *st);
int stmt_list_expand(struct stmt_list *list);
int stmt_list_clean(struct stmt_list *list);

// expr_stmt: expr stmt_trailer
struct expr_stmt {
    COBJECT
    struct expr *e;
};
struct expr_stmt *new_expr_stmt(struct expr *e);
struct expr_stmt *parse_expr_stmt(struct parser *p);

// var_stmt: 'var' expr '=' expr stmt_trailer
struct var_stmt {
    COBJECT
    struct token *ident;
    struct expr *value;
};
struct var_stmt *parse_var_stmt(struct parser *p, struct token *tk);

// return_stmt: 'return' expr stmt_trailer
struct return_stmt {
    COBJECT
    struct expr *value;
};
struct return_stmt *parse_return_stmt(struct parser *p, struct token *tk);

// if_stmt: 'if' '(' expr ')' block_stmt ['elif' '(' expr ')' block_stmt]* ['else' block_stmt]?
struct if_stmt {
    COBJECT
    struct token *lp;
    struct expr *cond;
    struct token *rp;
    struct block_stmt *then;
    struct stmt_list *elif_stmts;
    struct block_stmt *_else;
};
struct if_stmt *parse_if_stmt(struct parser *p, struct token *tk);

// block_stmt: '{' stmt* '}' stmt_trailer
struct block_stmt {
    COBJECT
    struct stmt_list *stmts;
    struct token *rbr;
};
struct block_stmt *parse_block_stmt(struct parser *p, struct token *tk);

// for_stmt: 'for' '(' [expr_list]; [expr]; [expr_list] ')' block_stmt
struct for_stmt {
    COBJECT
    struct token *lp;
    struct expr_list *init;
    struct expr *cond;
    struct expr_list *post;
    struct token *rp;
    struct block_stmt *stmts;
};
struct for_stmt *parse_for_stmt(struct parser *p, struct token *tk);

#endif
