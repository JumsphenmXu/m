#ifndef __M_AST_H__
#define __M_AST_H__

struct program {
    struct stmt_list *stmts;
};
struct program *new_program();
int init_program(struct program **prog, unsigned long stmt_num);
int add_stmt(struct program *prog, struct stmt *st);
struct stmt *get_stmt(struct program *prog, unsigned int i);

#define PROGRAM_SIZE(p) ((p)->stmts->size)

struct parser {
    struct program *prog;
    struct lexer *lex;
    struct token *cur, *next;
};
struct parser *new_parser();
int init_parser(struct parser **p, struct lexer *lex);
struct token *get_current_token(struct parser *p);
void advance_token(struct parser *p);
struct token *peek_next_token(struct parser *p);
int parse_from_string(struct parser *p, const char *str);
int parse_from_file(struct parser *p, const char *filename);
char *get_filename(struct parser *p);

#define HANDLE_TRAILING(p, tk)          \
    tk = peek_next_token(p);            \
    if (IS_TOKEN(tk, TK_SEMICOLON)) {   \
        advance_token(p);               \
    } else if (!IS_TOKEN(tk, TK_EOF) && !IS_TOKEN(tk, TK_NEWLINE)) { \
        EXPECT_TOKEN(tk, TK_SEMICOLON);   \
    }

#endif
