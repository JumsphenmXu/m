#include "include/m.h"

int main(int argc, char *argv[]) {
    /**
    struct token *t;
    const char *str = "var a = 123; func aa() { if (1) { val c = 3 } }";

    struct lexer *lex = new_lexer();
    if (init_lexer(lex, "./tokenizer.txt", str) < 0) {
        return -1;
    }

    printf("tokenizer start...\n");
    for (;;) {
        t = tokenizer(lex);
        if (t->type == INVALID) {
            printf("invalid token, line: %d, col: %d\n", lex->line, lex->col);
            break;
        }
        printf("%s: %s\n", token_expr(t->type), t->ident);
    }
    */

    if (argc < 2) {
        printf("please specify an input file\n");
        return 0;
    }

    struct parser *p = new_parser();
    struct stmt *st;
    int i = 0;
    // parse_from_file(p, "./parse_tpl.txt");
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
