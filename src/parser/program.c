#include "../include/m.h"


struct program *new_program() {
    struct program *p;
    p = (struct program *) malloc (sizeof(struct program));
    CHECK_NULL(p);
    return p;
}

int init_program(struct program **prog, unsigned long stmt_num) {
    struct program *ptr = *prog;
    ptr->stmts = new_stmt_list(stmt_num);
    CHECK_NULL_ERROR(ptr->stmts);
    RETURN_OK;
}

int add_stmt(struct program *prog, struct stmt *st) {
    return stmt_list_add(prog->stmts, st);
}

struct stmt *get_stmt(struct program *prog, unsigned int i) {
    return stmt_list_get(prog->stmts, i);
}
