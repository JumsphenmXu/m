#include "../include/m.h"

struct ibuiltin_func *new_ibuiltin_func(function fn) {
    struct ibuiltin_func *bltin;

    bltin = (struct ibuiltin_func *) malloc (sizeof (struct ibuiltin_func));
    CHECK_NULL(bltin);
    SET_IOBJECT_TYPE(bltin, OT_BUILTIN_FUNC);
    bltin->fn = fn;

    return bltin;
}

struct iobject *bltin_print(struct tw_interp *tw, struct iobject *obj) {
    int i;
    struct ilist_object *list;
    if (obj == NULL) {
        printf("NULL");
    } else if (IS_IOBJECT_TYPE(obj, OT_ARRAY)) {
        list = (struct ilist_object *) obj;
        for (i = 0; i < list->size; i++) {
            iobject_print(ilist_get(list, i));
            if (i < list->size - 1) {
                printf(" ");
            }
        }
        printf("\n");
    } else {
        printf("invalid parameter format to call builtin <print> function, array is expected\n");
    }
    RETURN_NIL;
}

struct iobject *bltin_type(struct tw_interp *interp, struct iobject *obj) {
    struct iobject *type;
    struct ilist_object *list;

    if (IS_IOBJECT_TYPE(obj, OT_ARRAY)) {
        list = (struct ilist_object *) obj;
        if (list->size != 1) {
            printf("builtin <type> function expect exactly 1 parameters, %ld is provided\v", list->size);
            RETURN_NIL;
        }
        obj = ilist_get(list, 0);
        obj = VALUE(obj);
    } else {
        printf("invalid parameter format to call builtin <type> function, array is expected\n");
        RETURN_NIL;
    }

    switch (IOBJECT_TYPE(obj)) {
        case OT_NIL:
            type = (struct iobject *) find_string_literal(GLOBAL_INTERP()->consts, "nil");
            break;
        case OT_CHAR:
            type = (struct iobject *) find_string_literal(GLOBAL_INTERP()->consts, "char");
            break;
        case OT_BOOL:
            type = (struct iobject *) find_string_literal(GLOBAL_INTERP()->consts, "bool");
            break;
        case OT_INT:
            type = (struct iobject *) find_string_literal(GLOBAL_INTERP()->consts, "int");
            break;
        case OT_FLOAT:
            type = (struct iobject *) find_string_literal(GLOBAL_INTERP()->consts, "float");
            break;
        case OT_STRING:
            type = (struct iobject *) find_string_literal(GLOBAL_INTERP()->consts, "string");
            break;
        case OT_MAP_ITEM:
            type = (struct iobject *) find_string_literal(GLOBAL_INTERP()->consts, "map_item");
            break;
        case OT_MAP:
            type = (struct iobject *) find_string_literal(GLOBAL_INTERP()->consts, "map");
            break;
        case OT_ARRAY:
            type = (struct iobject *) find_string_literal(GLOBAL_INTERP()->consts, "array");
            break;
        case OT_FUNC:
            type = (struct iobject *) find_string_literal(GLOBAL_INTERP()->consts, "func");
            break;
        case OT_ADDRESS:
            type = (struct iobject *) find_string_literal(GLOBAL_INTERP()->consts, "address");
            break;
        default:
            type = (struct iobject *) find_string_literal(GLOBAL_INTERP()->consts, "unknown");
            break;
    }
    return type;
}

struct iobject *bltin_len(struct tw_interp *interp, struct iobject *obj) {
    struct iobject *len;
    struct ilist_object *list;
    unsigned long l;

    if (!IS_IOBJECT_TYPE(obj, OT_ARRAY)) {
        printf("invalid parameter format for bulitin <len> function, array is expected\n");
        RETURN_NIL;
    }

    list = (struct ilist_object *) obj;
    if (list->size != 1) {
        printf("builtin <len> function expect exactly 1 parameters, %ld is provided\v", list->size);
        RETURN_NIL;
    }

    obj = ilist_get(list, 0);
    obj = VALUE(obj);
    if (IS_IOBJECT_TYPE(obj, OT_ARRAY)) {
        l = ((struct ilist_object *) obj)->size;
    } else if (IS_IOBJECT_TYPE(obj, OT_MAP)) {
        l = ((struct imap_object *) obj)->occupied_slot_cnt;
    } else {
        printf("builtin <len> function expected parameters of type ARRAY or MAP\n");
        RETURN_NIL;
    }

    len = (struct iobject *) find_int_literal(interp->consts, l);
    return len;
}

struct iobject *bltin_append(struct tw_interp *interp, struct iobject *obj) {
    struct iobject *v;
    struct ilist_object *list;

    if (!IS_IOBJECT_TYPE(obj, OT_ARRAY)) {
        printf("invalid parameter format for bulitin <append> function, array is expected\n");
        RETURN_NIL;
    }

    list = (struct ilist_object *) obj;
    if (list->size != 2) {
        printf("builtin <append> function expect exactly 2 parameters, %ld is provided\v", list->size);
        RETURN_NIL;
    }

    v = VALUE(ilist_get(list, 0));
    if (!IS_IOBJECT_TYPE(v, OT_ARRAY)) {
        printf("invalid parameter format for bulitin <append> function, the first parameter must be array\n");
        RETURN_NIL;
    }
    obj = VALUE(ilist_get(list, 1));
    list = (struct ilist_object *) v;
    if (ilist_add(list, obj) < 0) {
        RETURN_NIL;
    }

    return (struct iobject *) list;
}

struct iobject *bltin_del(struct tw_interp *interp, struct iobject *obj) {
    struct iobject *v;
    struct ilist_object *list;
    struct imap_object *map;

    if (!IS_IOBJECT_TYPE(obj, OT_ARRAY)) {
        printf("invalid parameter format for bulitin <del> function, array is expected\n");
        RETURN_NIL;
    }

    list = (struct ilist_object *) obj;
    if (list->size != 2) {
        printf("builtin <append> function expect exactly 2 parameters, %ld is provided\v", list->size);
        RETURN_NIL;
    }

    v = VALUE(ilist_get(list, 0));
    obj = VALUE(ilist_get(list, 1));
    if (IS_IOBJECT_TYPE(v, OT_ARRAY)) {
        list = (struct ilist_object *) v;
        if (ilist_del(list, obj) < 0) {
            RETURN_NIL;
        }
        v = (struct iobject *) list;
    } else if (IS_IOBJECT_TYPE(v, OT_MAP)) {
        map = (struct imap_object *) v;
        if (imap_del(map, obj) < 0) {
            RETURN_NIL;
        }
        v = (struct iobject *) map;
    } else {
        printf("invalid parameter format for bulitin <del> function, the first parameter must be array or map\n");
        RETURN_NIL;
    }

    return v;
}

int ibuiltin_init() {
    struct ibuiltin_func *print_fn, *type_fn, *len_fn, *append_fn, *del_fn;
    struct iliteral_object *literal;

    print_fn = new_ibuiltin_func(bltin_print);
    literal = find_string_literal(GLOBAL_INTERP()->consts, "print");
    CHECK_NULL_ERROR(literal);
    ENV_SET(GLOBAL_INTERP()->env, (struct iobject *) literal, (struct iobject *) print_fn);

    type_fn = new_ibuiltin_func(bltin_type);
    literal = find_string_literal(GLOBAL_INTERP()->consts, "type");
    CHECK_NULL_ERROR(literal);
    ENV_SET(GLOBAL_INTERP()->env, (struct iobject *) literal, (struct iobject *) type_fn);

    len_fn = new_ibuiltin_func(bltin_len);
    literal = find_string_literal(GLOBAL_INTERP()->consts, "len");
    CHECK_NULL_ERROR(literal);
    ENV_SET(GLOBAL_INTERP()->env, (struct iobject *) literal, (struct iobject *) len_fn);

    append_fn = new_ibuiltin_func(bltin_append);
    literal = find_string_literal(GLOBAL_INTERP()->consts, "append");
    CHECK_NULL_ERROR(literal);
    ENV_SET(GLOBAL_INTERP()->env, (struct iobject *) literal, (struct iobject *) append_fn);

    del_fn = new_ibuiltin_func(bltin_del);
    literal = find_string_literal(GLOBAL_INTERP()->consts, "del");
    CHECK_NULL_ERROR(literal);
    ENV_SET(GLOBAL_INTERP()->env, (struct iobject *) literal, (struct iobject *) del_fn);

    RETURN_OK;
}
