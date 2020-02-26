#ifndef _M_TW_INTERP_H__
#define _M_TW_INTERP_H__

enum tw_env_type {
    ET_INVALID_LWM,
    ET_GLOBAL,
    ET_FUNC,
    ET_FOR,
    ET_BLOCK,
    ET_INVALID_HWM,
};

struct tw_env {
    enum tw_env_type et;
    struct imap_object *map;
    struct tw_env *parent;
};

struct tw_env __env_;
struct tw_env *new_tw_env(struct tw_env *parent);
int tw_env_clean(struct tw_env *env);
struct iobject *tw_env_get(struct tw_env *env, struct iobject *key);
#define GLOBAL_ENV() (&__env_)
#define ENV_GET(e, k) tw_env_get((e), k)
#define ENV_SET(e, k, v) imap_set((e)->map, k, VALUE(v))
#define ENV_SET_TYPE(e, t) ((e)->et = t)
#define ASSERT_ENV_TYPE(e, t) if ((e)->et != t) exit(-1)
#define ENV_GET_TYPE(e) ((e)->et)
#define ENV_INVALID(e) ((e)->et <= ET_INVALID_LWM || (e)->et >= ET_INVALID_HWM)

struct tw_interp {
    struct tw_env *env;
    struct ilist_object *stk;
    struct imap_object *consts;
};

struct tw_interp *__interp_;
#define GLOBAL_INTERP() (__interp_)

int tw_push(struct tw_interp *interp, struct iobject *obj);
struct iobject *tw_top(struct tw_interp *interp);
void tw_pop(struct tw_interp *interp);
int tw_empty(struct tw_interp *interp);

int tw_eval(struct tw_interp *interp, struct program *prog);
int tw_eval_stmt(struct tw_interp *interp, struct stmt *st);
struct iobject *tw_eval_expr(struct tw_interp *interp, struct expr *e);

#define ENTER_ENV(interp, e, t)    \
    e = new_tw_env((interp)->env); \
    ASSERT_NOT_NULL(e);            \
    ENV_SET_TYPE(e, t);            \
    interp->env = e;

#define EXIT_ENV(interp, e)  \
    e = interp->env;         \
    interp->env = e->parent; \
    tw_env_clean(e);

int interp_init();

#endif
