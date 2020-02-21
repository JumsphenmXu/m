#include "../include/m.h"

static int _parse(struct parser *p);

struct parser *new_parser() {
    struct parser *p = (struct parser *) malloc (sizeof(struct parser));
    p->cur = p->next = NULL;
    return p;
}

int init_parser(struct parser **p, struct lexer *lex) {
    struct parser *ptr = *p;
    ptr->lex = lex;

    ptr->prog = new_program();
    if (ptr->prog == NULL) {
        RETURN_ERROR;
    }

    CHECK_ERROR(init_program(&ptr->prog, 80));

    // setup ptr->cur and ptr->next
    advance_token(ptr);
    advance_token(ptr);
    RETURN_OK;
}

struct token *get_current_token(struct parser *p) {
    return p->cur;
}

struct token *peek_next_token(struct parser *p) {
    return p->next;
}

void advance_token(struct parser *p) {
    struct token *tk;

    do {
        tk = tokenizer(p->lex);
        p->cur = p->next;
        p->next = tk;
    } while (p->cur && (IS_TOKEN(p->cur, TK_NEWLINE) || IS_TOKEN(p->cur, TK_COMMENT)));
}

int parse_from_string(struct parser *p, const char *str) {
    struct lexer *lex = new_lexer();
    if (lex == NULL) {
        RETURN_ERROR;
    }

    CHECK_ERROR(init_lexer(lex, NULL, str));
    CHECK_ERROR(init_parser(&p, lex));
    CHECK_ERROR(_parse(p));
    RETURN_OK;
}

int parse_from_file(struct parser *p, const char *filename) {
    struct lexer *lex = new_lexer();
    if (lex == NULL) {
        RETURN_ERROR;
    }

    CHECK_ERROR(init_lexer(lex, filename, NULL));
    CHECK_ERROR(init_parser(&p, lex));
    CHECK_ERROR(_parse(p));
    RETURN_OK;
}

static int _parse(struct parser *p) {
    struct token *tk;
    struct stmt *st;

    for (;;) {
        tk = get_current_token(p);
        switch (tk->type) {
            case TK_INVALID:
                RETURN_ERROR;
                break;
            case TK_EOF:
                RETURN_OK;
                break;
            default:
                st = (struct stmt *) parse_stmt(p, tk);
                break;
        }
        if (st) {
            CHECK_ERROR(add_stmt(p->prog, st));
        }
        advance_token(p);
    }
    RETURN_OK;
}

char *get_filename(struct parser *p) {
    if (p->lex->filename) {
        return p->lex->filename;
    }
    return "_m";
}
