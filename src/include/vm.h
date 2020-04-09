#ifndef _M_VM_H__
#define _M_VM_H__

struct vm {
    struct code *c;
    struct ilist_object *stk;
};

#endif
