#ifndef _M_BUILT_IN_H__
#define _M_BUILT_IN_H__

typedef struct iobject *(*function) (struct tw_interp *, struct iobject *);

struct ibuiltin_func {
    IOBJECT_HEAD
    function fn;
};

struct ibuiltin_func *new_ibuiltin_func(function fn);

struct iobject *bltin_print(struct tw_interp *interp, struct iobject *obj);
struct iobject *bltin_type(struct tw_interp *interp, struct iobject *obj);
struct iobject *bltin_append(struct tw_interp *interp, struct iobject *obj);
struct iobject *bltin_len(struct tw_interp *interp, struct iobject *obj);
struct iobject *bltin_del(struct tw_interp *interp, struct iobject *obj);

int ibuiltin_init();

#endif
