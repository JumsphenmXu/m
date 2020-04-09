#include "../include/m.h"

struct inst *new_inst(unsigned char opcode) {
    struct inst *i;
    i = (struct inst *) malloc (sizeof (struct inst));
    CHECK_NULL(i);
    SET_IOBJECT_TYPE(i, OT_INST);
    i->opcode = opcode;

    return i;
}

struct code *new_code() {
    struct code *c;
    c = (struct code *) malloc (sizeof (struct code));
    CHECK_NULL(c);
    SET_IOBJECT_TYPE(c, OT_CODE);

    return c;
}

int init_code(struct code *c) {
    if (c == NULL) {
        RETURN_ERROR;
    }

    c->insts = new_ilist();
    CHECK_NULL_ERROR(c->insts);
    c->consts = new_ilist();
    CHECK_NULL_ERROR(c->consts);
    c->consts_idx = new_imap();
    CHECK_NULL_ERROR(c->consts_idx);

    RETURN_OK;
}

int add_inst(struct code *c, struct inst *i) {
    return ilist_add(c->insts, (struct iobject *) i);
}

struct inst *get_inst(struct code *c, unsigned long idx) {
    struct inst *i;

    if (idx < 0 || idx >= c->insts->size) {
        printf("invalid instruction counter\n");
        return NULL;
    }
    i = (struct inst *) ilist_get(c->insts, idx);

    return i;
}

unsigned int add_const(struct code *c, struct iobject *obj) {
    unsigned int i;
    struct iobject *r;
    r = imap_get(c->consts_idx, obj);
    if (r) {
        r = VALUE(r);
        i = (unsigned int) (((struct iliteral_object *) r)->val.l);
    } else {
        if (ilist_add(c->consts, obj) < 0) {
            return -1;
        }
        i = c->consts->size - 1;
        if (imap_set(c->consts_idx, obj, (struct iobject *) new_int_literal(i)) < 0) {
            return -1;
        }
}

    return i;
}

struct iobject *get_const_o(struct code *c, struct iobject *idx) {
    if (!IS_IOBJECT_TYPE(idx, OT_INT)) {
        printf("get const, index is not int\n");
        return NULL;
    }

    return get_const_i(c, ((struct iliteral_object *) idx)->val.l);
}

struct iobject *get_const_i(struct code *c, unsigned long idx) {
    if (idx <= 0 || idx >= c->consts->size) {
        printf("get const, index out of range\n");
        return NULL;
    }

    return ilist_get(c->consts, idx);
}
