#include "../include/m.h"

struct imap_item_object *new_imap_item(struct iobject *key, struct iobject *val);
static int imap_rehash(struct imap_object *map);
static int __imap_add(struct imap_object *map, struct iobject *key, struct iobject *val, int force);

static int iliteral_equals(struct iobject *o1, struct iobject *o2);
static int iliteral_less_than(struct iobject *o1, struct iobject *o2);
static int need_rehash(struct imap_object *map);
static int ilist_expand(struct ilist_object *list);
static struct iliteral_object *new_iliteral();

int iobject_init() {
    SET_IOBJECT_TYPE(&inil, OT_NIL);
    SET_IOBJECT_TYPE(&itrue, OT_BOOL);
    SET_IOBJECT_TYPE(&ifalse, OT_BOOL);
    RETURN_OK;
}

/////// object //////////
struct iobject *new_iaddress(struct iobject *obj) {
    struct iaddress_object *iaddr;

    iaddr = (struct iaddress_object *) malloc (sizeof (struct iaddress_object));
    CHECK_NULL(iaddr);
    SET_IOBJECT_TYPE(iaddr, OT_ADDRESS);
    iaddr->addr = obj;

    return (struct iobject *) iaddr;
}

unsigned long get_iobject_hash(struct iobject *obj) {
    unsigned long hash;
    long h;
    char ch;
    char *data;
    struct iliteral_object *literal;

    hash = INVALID_HASH_VALUE;
    if (obj == NULL) {
        printf("NULL object can't be hashed\n");
        exit(-1);
    }

    switch (IOBJECT_TYPE(obj)) {
        case OT_INT:
        case OT_FLOAT:
            literal = (struct iliteral_object *) obj;
            h = GET_ILITERAL_VALUE(literal, l);
            if (h < 0) {
                h = -h;
            }
            hash = h;
            break;
        case OT_BOOL:
            hash = IS_TRUE(obj) ? 1 : 0;
            break;
        case OT_CHAR:
            literal = (struct iliteral_object *) obj;
            ch = GET_ILITERAL_VALUE(literal, c);
            hash = (unsigned long) ch;
            break;
        case OT_STRING:
            literal = (struct iliteral_object *) obj;
            data = GET_ILITERAL_VALUE(literal, s);
            hash = fnv64(data, strlen(data));
            break;
        default:
            if (obj->hfunc) {
                hash = ((struct iliteral_object *) (obj->hfunc(obj)))->val.l;
            } else {
                printf("can not be used as map key\n");
            }
            break;
    }

    return hash;
}

static int iliteral_equals(struct iobject *o1, struct iobject *o2) {
    struct iliteral_object *io1, *io2;

    io1 = (struct iliteral_object *) o1;
    io2 = (struct iliteral_object *) o2;
    int rc = 0;

    if (!SAME_TYPE(io1, io2)) {
        return rc;
    }

    switch (IOBJECT_TYPE(o1)) {
        case OT_CHAR:
            rc = io1->val.c == io2->val.c;
            break;
        case OT_INT:
        case OT_FLOAT:
            rc = io1->val.l == io2->val.l;
            break;
        case OT_STRING:
            rc = strcmp(io1->val.s, io2->val.s) == 0;
            break;
        default:
            break;
    }

    return rc;
}

static int iliteral_less_than(struct iobject *o1, struct iobject *o2) {
    struct iliteral_object *io1, *io2;

    io1 = (struct iliteral_object *) o1;
    io2 = (struct iliteral_object *) o2;
    int rc = 0;

    if (SAME_OBJ(io1, io2)) {
        return rc;
    }

    switch (IOBJECT_TYPE(o1)) {
        case OT_CHAR:
            rc = io1->val.c < io2->val.c;
            break;
        case OT_INT:
            rc = io1->val.l < io2->val.l;
            break;
        case OT_FLOAT:
            rc = io1->val.f < io2->val.f;
            break;
        case OT_STRING:
            rc = strcmp(io1->val.s, io2->val.s) < 0;
            break;
        default:
            break;
    }

    return rc;
}


int iobject_equals(struct iobject *l, struct iobject *r) {
    int rc;
    if (IS_IOBJECT_LITERAL(l) && IS_IOBJECT_LITERAL(r)) {
        rc = iliteral_equals(l, r);
    } else {
        rc = SAME_OBJ(l, r);
    }
    return rc;
}

int iobject_less_than(struct iobject *l, struct iobject *r) {
    int rc;
    if (IS_IOBJECT_LITERAL(l) && IS_IOBJECT_LITERAL(r)) {
        rc = iliteral_less_than(l, r);
    } else {
        rc = *((long *) l) < *((long *) r);
    }
    return rc;
}

static struct iliteral_object *new_iliteral() {
    struct iliteral_object *literal;
    literal = (struct iliteral_object *) malloc (sizeof (struct iliteral_object));
    return literal;
}

struct iliteral_object *new_string_literal(char *s) {
    struct iliteral_object *literal;

    literal = new_iliteral();
    CHECK_NULL(literal);
    SET_IOBJECT_TYPE(literal, OT_STRING);
    m_strndup(&(literal->val.s), s, strlen(s));

    return literal;
}

struct iliteral_object *new_int_literal(long l) {
    struct iliteral_object *literal;

    literal = new_iliteral();
    CHECK_NULL(literal);
    SET_IOBJECT_TYPE(literal, OT_INT);
    literal->val.l = l;

    return literal;
}

struct iliteral_object *new_float_literal(double f) {
    struct iliteral_object *literal;

    literal = new_iliteral();
    CHECK_NULL(literal);
    SET_IOBJECT_TYPE(literal, OT_FLOAT);
    literal->val.f = f;

    return literal;
}

struct iliteral_object *new_char_literal(char c) {
    struct iliteral_object *literal;

    literal = new_iliteral();
    CHECK_NULL(literal);
    SET_IOBJECT_TYPE(literal, OT_CHAR);
    literal->val.c = c;

    return literal;
}

struct iliteral_object *find_string_literal(struct imap_object *map, char *s) {
    unsigned long h;
    struct imap_item_object *item, *add_item;
    struct iliteral_object *obj, *res;

    h = fnv64(s, strlen(s));
    item = map->items[h%map->cap];
    res = NULL;
    while (item) {
        if (IS_IOBJECT_TYPE(item->key, OT_STRING)) {
            obj = (struct iliteral_object *) item->key;
            if (strcmp(obj->val.s, s) == 0) {
                res = obj;
                break;
            }
        }
        item = item->next;
    }

    if (res == NULL) {
        res = new_string_literal(s);
        SET_IOBJECT_HASH(res, h);
        if (res) {
            add_item = new_imap_item((struct iobject *) res, NULL);
            item = map->items[h%map->cap];
            if (add_item) {
                if (item) {
                    add_item->next = item;
                }
            }
            map->items[h%map->cap] = add_item;
        }
    }

    return res;
}
struct iliteral_object *find_int_literal(struct imap_object *map, long l) {
    struct imap_item_object *item, *add_item;
    struct iliteral_object *obj, *res;

    item = map->items[l%map->cap];
    res = NULL;
    while (item) {
        if (IS_IOBJECT_TYPE(item->key, OT_INT)) {
            obj = (struct iliteral_object *) item->key;
            if (obj->val.l == l) {
                res = obj;
                break;
            }
        }
        item = item->next;
    }

    if (res == NULL) {
        res = new_int_literal(l);
        SET_IOBJECT_HASH(res, l);
        if (res) {
            add_item = new_imap_item((struct iobject *) res, NULL);
            item = map->items[l%map->cap];
            if (add_item) {
                if (item) {
                    add_item->next = item;
                }
            }
            map->items[l%map->cap] = add_item;
        }
    }

    return res;
}

struct iliteral_object *find_float_literal(struct imap_object *map, double f) {
    union {
        long l;
        double f;
    } val;
    val.f = f;

    struct imap_item_object *item, *add_item;
    struct iliteral_object *obj, *res;

    item = map->items[val.l%map->cap];
    res = NULL;
    while (item) {
        if (IS_IOBJECT_TYPE(item->key, OT_FLOAT)) {
            obj = (struct iliteral_object *) item->key;
            if (obj->val.f == f) {
                res = obj;
                break;
            }
        }
        item = item->next;
    }

    if (res == NULL) {
        res = new_float_literal(f);
        SET_IOBJECT_HASH(res, val.l);
        if (res) {
            add_item = new_imap_item((struct iobject *) res, NULL);
            item = map->items[val.l%map->cap];
            if (add_item) {
                if (item) {
                    add_item->next = item;
                }
            }
            map->items[val.l%map->cap] = add_item;
        }
    }

    return res;
}

struct iliteral_object *find_char_literal(struct imap_object *map, char c) {
    struct imap_item_object *item, *add_item;
    struct iliteral_object *obj, *res;
    unsigned long l;

    l = (unsigned long) c;
    item = map->items[l%map->cap];
    res = NULL;
    while (item) {
        if (IS_IOBJECT_TYPE(item->key, OT_CHAR)) {
            obj = (struct iliteral_object *) item->key;
            if (obj->val.c == c) {
                res = obj;
                break;
            }
        }
        item = item->next;
    }

    if (res == NULL) {
        res = new_char_literal(c);
        SET_IOBJECT_HASH(res, l);
        if (res) {
            add_item = new_imap_item((struct iobject *) res, NULL);
            item = map->items[l%map->cap];
            if (add_item) {
                if (item) {
                    add_item->next = item;
                }
            }
            map->items[l%map->cap] = add_item;
        }
    }

    return res;
}

/////// map object /////////////
struct imap_item_object *new_imap_item(struct iobject *key, struct iobject *val) {
    struct imap_item_object *item;

    item = (struct imap_item_object *) malloc (sizeof (struct imap_item_object));
    CHECK_NULL(item);
    SET_IOBJECT_TYPE(item, OT_MAP_ITEM);
    item->key = key;
    item->val = val;
    item->next = NULL;

    return item;
}

struct imap_object *new_imap() {
    struct imap_object *map;
    unsigned long i, init_cap;

    map = (struct imap_object*) malloc (sizeof (struct imap_object));
    CHECK_NULL(map);
    SET_IOBJECT_TYPE(map, OT_MAP);

    init_cap = (unsigned long) get_next_map_capacity(0);
    map->items = (struct imap_item_object **) malloc (init_cap * sizeof (struct imap_item_object *));
    for (i = 0; i < init_cap; i++) {
        map->items[i] = NULL;
    }
    CHECK_NULL(map->items);
    map->size = 0;
    map->cap = init_cap;

    return map;
}

static int need_rehash(struct imap_object *map) {
    float lf1, lf2;

    lf1 = (float) map->occupied_slot_cnt / (float) map->cap;
    lf2 = (float) map->size / (float) map->cap;

    return lf1 >= HASH_LOAD_FACTOR || lf2 >= HASH_LOAD_FACTOR;
}

static int __imap_add(struct imap_object *map, struct iobject *key, struct iobject *val, int force) {
    unsigned long hash;
    int idx;
    struct imap_item_object *item;
    struct iobject *res;

    res = imap_get(map, key);

    if (res != NULL) {
        if (!force) {
            RETURN_ERROR;
        } else {
            SET_ADDR_VALUE(res, val);
            RETURN_OK;
        }
    }

    if (IS_HASH_INVALID(key)) {
        hash = get_iobject_hash(key);
        key->hash = hash;
    } else {
        hash = key->hash;
    }

    map->size++;
    item = new_imap_item(key, val);
    idx = hash % map->cap;
    if (map->items[idx]) {
        item->next = map->items[idx];
    } else {
        map->occupied_slot_cnt++;
    }
    map->items[idx] = item;

    if (need_rehash(map)) {
        return imap_rehash(map);
    }

    RETURN_OK;
}

int imap_add(struct imap_object *map, struct iobject *key, struct iobject *val) {
    return __imap_add(map, key, val, 0);
}

int imap_set(struct imap_object *map, struct iobject *key, struct iobject *val) {
    return __imap_add(map, key, val, 1);
}

int imap_del(struct imap_object *map, struct iobject *key) {
    unsigned long hash;
    int idx;
    struct imap_item_object *prev_item, *item;

    if (IS_HASH_INVALID(key)) {
        hash = get_iobject_hash(key);
        key->hash = hash;
    } else {
        hash = key->hash;
    }

    idx = hash % map->cap;
    item = NULL;
    prev_item = NULL;
    if ((item = map->items[idx])) {
        while (item) {
            if (SAME_OBJ(item->key, key)) {
                break;
            }

            if (IS_LITERAL_IOBJECT(key) &&
                    IS_LITERAL_IOBJECT(item->key) &&
                    iliteral_equals(item->key, key)) {
                break;
            } else {
                // TODO: more types
            }

            prev_item = item;
            item = item->next;
        }
    }

    if (item) {
        if (prev_item) {
            prev_item->next = item->next;
        } else {
            map->items[idx] = item->next;
        }
        item->next = NULL;
        free(item);
    }

    RETURN_OK;
}

struct iobject *imap_get(struct imap_object *map, struct iobject *key) {
    unsigned long hash;
    int idx;
    struct iobject *obj;
    struct imap_item_object *item;

    if (IS_HASH_INVALID(key)) {
        hash = get_iobject_hash(key);
        key->hash = hash;
    } else {
        hash = key->hash;
    }

    idx = hash % map->cap;
    item = NULL;
    if ((item = map->items[idx])) {
        while (item) {
            if (SAME_OBJ(item->key, key)) {
                break;
            }

            if (IS_LITERAL_IOBJECT(key) &&
                    IS_LITERAL_IOBJECT(item->key) &&
                    iliteral_equals(item->key, key)) {
                break;
            } else {
                // TODO: more types
            }

            item = item->next;
        }
    }

    obj = NULL;
    if (item) {
        // TODO: address object not work
        if (item->val && IS_IOBJECT_TYPE(item->val, OT_ADDRESS)) {
            obj = item->val;
        } else {
            obj = new_iaddress((struct iobject *) item);
        }
    }

    return obj;
}

struct iobject *imap_get_or_create(struct imap_object *map, struct iobject *key) {
    struct iobject *obj;

    obj = imap_get(map, key);
    if (obj == NULL) {
        if (imap_set(map, key, NULL) < 0) {
            return NULL;
        }

        obj = imap_get(map, key);
    }

    return obj;
}

static int imap_rehash(struct imap_object *map) {
    unsigned long idx, new_idx, new_cap;
    struct imap_item_object *cur, *next, **items;

    new_cap = (unsigned long) get_next_map_capacity(map->cap);
    if (new_cap == map->cap) {
        printf("can not expand map, rehash failed\n");
        RETURN_ERROR;
    }

    items = (struct imap_item_object **) malloc (new_cap * sizeof (struct imap_item_object *));
    CHECK_NULL_ERROR(items);
    for (idx = 0; idx < new_cap; idx++) {
        items[idx] = NULL;
    }

    for (idx = 0; idx < map->cap; idx++) {
        cur = map->items[idx];
        while (cur) {
            next = cur->next;
            new_idx = cur->key->hash % new_cap;
            if (items[new_idx]) {
                cur->next = items[new_idx];
            }
            items[new_idx] = cur;
            cur = next;
        }
    }
    free(map->items);
    map->items = items;
    map->cap = new_cap;

    RETURN_OK;
}

//////////// list object ///////////////////
//
struct ilist_object *new_ilist() {
    struct ilist_object *list;
    unsigned long cap;

    list = (struct ilist_object *) malloc (sizeof (struct ilist_object));
    CHECK_NULL(list);
    SET_IOBJECT_TYPE(list, OT_ARRAY);

    cap = INIT_SEQ_LEN;
    list->elems = (struct iobject **) malloc (cap * sizeof(struct iobject));
    CHECK_NULL(list->elems);
    list->cap = cap;
    list->size = 0;

    return list;
}

int ilist_add(struct ilist_object *list, struct iobject *obj) {
    if (list->size >= list->cap - 1) {
        if (ilist_expand(list) < 0) {
            RETURN_ERROR;
        }
    }
    list->elems[list->size++] = obj;
    RETURN_OK;
}

int ilist_del(struct ilist_object *list, struct iobject *idx) {
    int i, j;
    if (!IS_IOBJECT_TYPE(idx, OT_INT)) {
        RETURN_ERROR;
    }

    i = GET_ILITERAL_VALUE((struct iliteral_object *) idx, l);
    if (i < 0 || i >= list->size) {
        RETURN_ERROR;
    }

    j = i + 1;
    while (j < list->size) {
        list->elems[i] = ilist_get(list, j);
        i++;
        j++;
    }
    list->size--;

    RETURN_OK;
}

struct iobject *ilist_get(struct ilist_object *list, int i) {
    struct iobject *obj = NULL;

    if (i < list->size) {
        obj = list->elems[i];
    } else {
        printf("access ilist: index out of range, idx: %d, array length: %d\n", i, (int) list->size);
    }

    return obj;
}

struct iobject *ilist_get_range(struct ilist_object *list, int i, int j) {
    struct ilist_object *rlist;

    if (i < 0 || i > j || j >= list->size) {
        return NULL;
    }

    rlist = new_ilist();
    for (; i < j; i++) {
        ilist_add(rlist, ilist_get(list, i));
    }

    return (struct iobject *) rlist;
}

static int ilist_expand(struct ilist_object *list) {
    int new_cap, size;

    if (list == NULL) {
        RETURN_ERROR;
    }

    new_cap = list->cap * 7 / 4;
    if (new_cap == list->cap) {
        new_cap = 2 * new_cap + 1;
    }
    size = list->size;

    list->elems = (struct iobject **) realloc (list->elems, new_cap * sizeof (struct iobject *));
    CHECK_NULL_ERROR(list->elems);
    list->cap = new_cap;
    list->size = size;

    RETURN_OK;
}

int ilist_set(struct ilist_object *list, struct iobject *idx, struct iobject *obj) {
    int i;
    if (!IS_IOBJECT_TYPE(idx, OT_INT)) {
        RETURN_ERROR;
    }

    i = GET_ILITERAL_VALUE((struct iliteral_object *) idx, l);
    if (i < 0 || i >= list->size) {
        RETURN_ERROR;
    }

    list->elems[i] = obj;

    RETURN_OK;
}

///////////////////// func object ////////////////
struct ifunc_object *new_ifunc_object(struct func_expr *fn) {
    struct ifunc_object *fo;

    fo = (struct ifunc_object *) malloc (sizeof (struct ifunc_object));
    CHECK_NULL(fo);
    SET_IOBJECT_TYPE(fo, OT_FUNC);
    fo->fn = fn;

    return fo;
}

///////////////// print ///////////////
static void iobject_print_map_item(struct imap_item_object *obj);
static void iobject_print_map(struct imap_object *map);
static void iobject_print_array(struct ilist_object *list);
static void iobject_print_func(struct ifunc_object *func);
static void iobject_print_address(struct iaddress_object *addr);

void iobject_print(struct iobject *obj) {
    if (obj == NULL) {
        printf("NULL\n");
        return;
    }

    obj = VALUE(obj);
    switch (IOBJECT_TYPE(obj)) {
        case OT_NIL:
            printf("nil");
            break;
        case OT_CHAR:
            printf("%c", ((struct iliteral_object *) obj)->val.c);
            break;
        case OT_INT:
            printf("%ld", ((struct iliteral_object *) obj)->val.l);
            break;
        case OT_FLOAT:
            printf("%.2f", ((struct iliteral_object *) obj)->val.f);
            break;
        case OT_BOOL:
            printf("%s", (IS_TRUE(obj) ? "true" : "false"));
            break;
        case OT_STRING:
            printf("%s", ((struct iliteral_object *) obj)->val.s);
            break;
        case OT_MAP_ITEM:
            iobject_print_map_item((struct imap_item_object *) obj);
            break;
        case OT_MAP:
            iobject_print_map((struct imap_object *) obj);
            break;
        case OT_ARRAY:
            iobject_print_array((struct ilist_object *) obj);
            break;
        case OT_FUNC:
            iobject_print_func((struct ifunc_object *) obj);
            break;
        case OT_ADDRESS:
            iobject_print_address((struct iaddress_object *) obj);
            break;
        default:
            break;
    }
}

static void iobject_print_map_item(struct imap_item_object *item) {
    printf("<");
    printf(" key: ");
    iobject_print(item->key);
    printf(" ,val: ");
    iobject_print(item->val);
    printf(">\n");
}

static void iobject_print_map(struct imap_object *map) {
    int i;
    struct imap_item_object *item;
    printf("{\n");
    for (i = 0; i < map->cap; i++) {
        item = map->items[i];
        while (item) {
            printf("\tkey: ");
            iobject_print(item->key);
            printf(", val: ");
            iobject_print(item->val);
            item = item->next;
        }
    }
    printf("\n}\n");
}

static void iobject_print_array(struct ilist_object *list) {
    int i;
    printf("[\n");
    for (i = 0; i < list->size; i++) {
        printf("\telem:");
        iobject_print(VALUE(ilist_get(list, i)));
    }
    printf("\n]\n");
}

static void iobject_print_func(struct ifunc_object *func) {
    struct func_expr *fn = func->fn;
    printf("func\n");
}

static void iobject_print_address(struct iaddress_object *addr) {
    printf("address: ");
    iobject_print(((struct imap_item_object *) (addr->addr))->val);
}
