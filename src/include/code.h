#ifndef _M_CODE_H__
#define _M_CODE_H__


struct inst {
    IOBJECT_HEAD
    unsigned char opcode;
    unsigned short p1, p2, p3;
};

struct code {
    IOBJECT_HEAD
    struct ilist_object *insts;
    struct ilist_object *consts;
    struct imap_object *consts_idx;
};

struct inst *new_inst(unsigned char opcode);
int add_inst(struct code *c, struct inst *i);
struct inst *get_inst(struct code *c, unsigned long idx);

struct code *new_code();
int init_code(struct code *c);

unsigned int add_const(struct code *c, struct iobject *obj);
struct iobject *get_const_o(struct code *c, struct iobject *idx);
struct iobject *get_const_i(struct code *c, unsigned long idx);


#endif
