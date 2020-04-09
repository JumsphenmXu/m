#ifndef _M_COMPILER_H__
#define _M_COMPILER_H__

struct compiler {
    struct code *c;
    struct ilist_object *scope;
};

struct compiler *new_compiler();
int init_compiler(struct compiler *c);
int compile(struct program *prog);

#endif
