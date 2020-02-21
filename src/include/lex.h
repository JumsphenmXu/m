#ifndef _M_LEX_H__
#define _M_LEX_H__

#define M_EOF ((char)0)
#define WHITESPACE(ch) ((ch) == ' ' || (ch) == '\t' || (ch) == '\b')
#define DIGIT(ch) ((ch) >= '0' && (ch) <= '9')
#define ALPHANUM(ch) ((ch) >= 'a' && (ch) <= 'z' || (ch) >= 'A' && (ch) <= 'Z' || (ch) == '_' || DIGIT(ch))
#define LEX_BUF_LEN 1024

struct lexer {
    FILE *fp;
    char *filename;
    char buf[2][LEX_BUF_LEN];
    char *str;
    int idx, filled;
    char *cur, *end;
    int line, col;
};

struct lexer *new_lexer();
int init_lexer(struct lexer *lex, const char *filename, const char *str);
void skip(struct lexer *lex);
char next(struct lexer *lex);
char peek(struct lexer *lex);
void back(struct lexer *lex);
struct token *tokenizer(struct lexer *lex);

const char *token_expr(int type);
char *get_token_info(const struct token *tk);
int find_keyword(const char *ident);

#endif
