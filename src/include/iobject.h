#ifndef _M_IOBJECT_H__
#define _M_IOBJECT_H__

/*
 * internal object definition for tree-walking-interpreter
 *
 **/

#define INIT_SEQ_LEN 7

enum iobject_type {
    OT_NIL,
    OT_ADDRESS,
    OT_CHAR,
    OT_INT,
    OT_FLOAT,
    OT_STRING,
    OT_BOOL,
    OT_ARRAY,
    OT_MAP_ITEM,
    OT_MAP,
    OT_FUNC,
    OT_BUILTIN_FUNC,
    OT_ENV,
};

struct iobject;
struct tw_interp;
struct imap_object;

#define F_ASSIGNABLE (1 << 0)

typedef struct iobject *(*unary_func) (struct iobject *);

#define IOBJECT_HEAD         \
    enum iobject_type type;  \
    unsigned long hash, flag;\
    unary_func hfunc;

#define INVALID_HASH_VALUE (0)
#define IS_HASH_INVALID(o) ((o)->hash == INVALID_HASH_VALUE)

#define IOBJECT_TYPE(o) ((o)->type)
#define IOBJECT_HASH(o) ((o)->hash)
#define IOBJECT_FLAG(o) ((o)->flag)
#define IOBJECT_HASH_FUNC(o) ((o)->hfunc)

#define SET_IOBJECT_TYPE(o, t) ((o)->type = t)
#define SET_IOBJECT_HASH(o, h) ((o)->hash = h)
#define SET_IOBJECT_FLAG(o, f) ((o)->flag |= (f))
#define CLEAR_IOBJECT_FLAG(o, f) ((o)->flag &= ~(f))
#define SET_IOBJECT_HASH_FUNC(o, hf) ((o)->hfunc = hf)

#define SET_IOBJECT_ASSIGNABLE(o) SET_IOBJECT_FLAG(o, F_ASSIGNABLE)
#define CLEAR_IOBJECT_ASSIGNABLE(o) CLEAR_IOBJECT_FLAG(o, F_ASSIGNABLE)
#define IOBJECT_ASSIGNABLE(o) ((o)->flag & F_ASSIGNABLE != 0)

#define IS_IOBJECT_TYPE(o, t) ((o)->type == t)
#define IS_LITERAL_IOBJECT(o) ((o)->type >= OT_CHAR && (o)->type <= OT_STRING)

struct iobject {
    IOBJECT_HEAD
};
struct iobject inil, itrue, ifalse;

struct iaddress_object {
    IOBJECT_HEAD
    struct iobject *addr;
};
struct iobject *new_iaddress(struct iobject *obj);
#define ADDRESSABLE(o) ((o) && IOBJECT_TYPE((o)) == OT_ADDRESS)
#define ADDR(o) ((struct iaddress_object *) (o))
#define ADDR_FIELD(o) ((struct imap_item_object *) (ADDR(o)->addr))
#define VALUE(o) (ADDRESSABLE(o) ? (ADDR_FIELD(o)->val) : (o))
#define SET_ADDR_VALUE(o, v) (((struct imap_item_object *) ((struct iaddress_object *) (o))->addr)->val = v)

#define IS_NIL(o) ((o) == &inil)
#define IS_TRUE(o) ((o) == &itrue)
#define IS_FALSE(o) ((o) == &inil || (o) == &ifalse)

#define RETURN_NIL return &inil
#define RETURN_TRUE return &itrue
#define RETURN_FALSE return &ifalse

#define SET_NIL(o) (o) = &inil
#define SET_TRUE(o) (o) = &itrue
#define SET_FALSE(o) (o) = &ifalse

#define SAME_OBJ(o1, o2) ((o1) == (o2))
#define SAME_TYPE(o1, o2) ((o1)->type == (o2)->type)

struct iliteral_object {
    IOBJECT_HEAD
    union {
        char c; // char
        long l; // int
        double f; // float
        char *s; // string
    } val;
};
#define GET_ILITERAL_VALUE(i, v) ((i)->val.v)
#define SET_ILITERAL_VALUE(i, v, e) ((i)->val.v = e)
#define OT(o, t) IS_IOBJECT_TYPE((o), (t))
#define IS_IOBJECT_LITERAL(o) (OT((o), OT_CHAR) || OT((o), OT_INT) || OT((o), OT_FLOAT) || OT((o), OT_STRING))
struct iliteral_object *new_string_literal(char *s);
struct iliteral_object *new_int_literal(long l);
struct iliteral_object *new_float_literal(double f);
struct iliteral_object *new_char_literal(char c);
struct iliteral_object *find_string_literal(struct imap_object *map, char *s);
struct iliteral_object *find_int_literal(struct imap_object *map, long l);
struct iliteral_object *find_float_literal(struct imap_object *map, double f);
struct iliteral_object *find_char_literal(struct imap_object *map, char c);

struct ilist_object {
    IOBJECT_HEAD
    unsigned long size, cap;
    struct iobject **elems;
};
struct ilist_object *new_ilist();
int ilist_add(struct ilist_object *list, struct iobject *obj);
int ilist_del(struct ilist_object *list, struct iobject *idx);
int ilist_set(struct ilist_object *list, struct iobject *idx, struct iobject *obj);
struct iobject *ilist_get(struct ilist_object *list, int i);
struct iobject *ilist_get_range(struct ilist_object *list, int i, int j);
#define GET_LIST_ELEM(l, i) ((l)->elems[i])
#define SET_LIST_ELEM(l, i, e) ((l)->elems[i] = e)

struct imap_item_object {
    IOBJECT_HEAD
    struct iobject *key;
    struct iobject *val;
    struct imap_item_object *next;
};
struct imap_item_object *new_imap_item(struct iobject *key, struct iobject *val);

#define GET_MAP_ITEM(m, i) ((m)->items[i])
#define SET_MAP_ITEM(m, i, t) ((m)->items[i] = t)
#define GET_MAP_ITEM_KEY(i) ((i)->key)
#define SET_MAP_ITEM_KEY(i, k) ((i)->key = k)
#define SET_MAP_ITEM_VALUE(i, v) ((i)->val = v)
#define HASH_LOAD_FACTOR (0.75)
struct imap_object {
    IOBJECT_HEAD
    unsigned long occupied_slot_cnt, size, cap;
    struct imap_item_object **items;
};
struct imap_object *new_imap();
int imap_add(struct imap_object *map, struct iobject *key, struct iobject *val);
int imap_set(struct imap_object *map, struct iobject *key, struct iobject *val);
int imap_del(struct imap_object *map, struct iobject *key);
struct iobject *imap_get(struct imap_object *map, struct iobject *key);
struct iobject *imap_get_or_create(struct imap_object *map, struct iobject *key);

struct func_expr;
struct ifunc_object {
    IOBJECT_HEAD
    struct func_expr *fn;
};
struct ifunc_object *new_ifunc_object(struct func_expr *fn);

int iobject_init();
unsigned long get_iobject_hash(struct iobject *obj);
void iobject_print(struct iobject *obj);
int iobject_equals(struct iobject *l, struct iobject *r);
int iobject_less_than(struct iobject *l, struct iobject *r);

#endif
