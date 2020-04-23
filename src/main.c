#include "include/m.h"


static int _proc(char *s, int is_file) {
    struct parser *p = new_parser();
    struct stmt *st;
    int i = 0;
    if (is_file) {
        parse_from_file(p, s);
    } else {
        parse_from_string(p, s);
    }

    CHECK_NULL_ERROR(p->prog);
    CHECK_NULL_ERROR(p->prog->stmts);

    printf("output stmts:\n");
    for (; i < PROGRAM_SIZE(p->prog); i++) {
        st = get_stmt(p->prog, i);
        print_stmt(st, 0);
    }

    iobject_init();
    interp_init();

    printf("\n\n\neval stmts:\n");
    tw_eval(GLOBAL_INTERP(), p->prog);

    return 0;
}


int main(int argc, char *argv[]) {
    if (argc >= 2) {
        return _proc(argv[1], 1);
    }

    char buf[2048];
    while (1) {
        printf(">>> ");
        memset(buf, 0, 2048);
        fgets(buf, 2048, stdin);
        _proc(buf, 0);
    }

    return 0;
}
