#include "include/m.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("please specify an input file\n");
        return 0;
    }

    struct parser *p = new_parser();
    struct stmt *st;
    int i = 0;
    parse_from_file(p, argv[1]);

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
