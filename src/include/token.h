#ifndef _M_TOKEN_H__
#define _M_TOKEN_H__

#define INIT_TOKEN_LEN 20
#define MAX_TOKEN_LEN 256
#define INIT_TOKEN_LIST_SIZE 8

#define EXPECT_TOKEN(tk, typ) do       \
    {                                  \
        if ((tk)->type != (typ)) {     \
            printf("Error: line %d, column %d expect %s, got %s, current parsed: \"%s\"\n", \
                    tk->line, tk->col, token_expr(typ), token_expr(tk->type),\
                    tk->literal ? tk->literal : "NULL"); \
            exit(-1);                  \
        }                              \
    } while (0)

#define IS_TOKEN(tk, typ) ((tk)->type == (typ))

enum token_type {
    TK_INVALID,
    TK_COMMENT, // #
    TK_EOF,
    TK_NEWLINE,

    TK_CHAR,
    TK_INT,
    TK_FLOAT,
    TK_STRING,
    TK_IDENT,

    TK_VAR, // var
    TK_VAL, // val
    TK_IF, // if
    TK_ELSEIF, // elif
    TK_ELSE, // else
    TK_FOR, // for
    TK_FUNC, // func
    TK_RET, // return
    TK_TRUE, // true
    TK_FALSE, // false
    TK_NIL, // nil

    TK_SEMICOLON, // ;
    TK_COLON, // :
    TK_COMMA, // ,
    TK_DOT, // .

    TK_ASSIGN, // =
    TK_EQ, // ==
    TK_NOT, // !
    TK_NE, // !=
    TK_LT, // <
    TK_LSHIFT, // <<
    TK_LSHIFT_ASSIGN, // <<=
    TK_LE, // <=
    TK_GT, // >
    TK_RSHIFT, // >>
    TK_RSHIFT_ASSIGN, // >>=
    TK_GE, // >=
    TK_AND, // &
    TK_AND_ASSIGN, // &=
    TK_OR, // |
    TK_OR_ASSIGN, // |=
    TK_XOR, // ^
    TK_XOR_ASSIGN, // ^=
    TK_PLUS, // +
    TK_PLUS_ASSIGN, // +=
    TK_MINUS, // -
    TK_MINUS_ASSIGN, // -=
    TK_STAR, // *
    TK_STAR_ASSIGN, // *=
    TK_SLASH, // /
    TK_SLASH_ASSIGN, // /=
    TK_PERCENT, // %
    TK_PERCENT_ASSIGN, // %=

    TK_NEGATE, // ~
    TK_LAND, // &&
    TK_LOR, // ||
    TK_LPAR, // (
    TK_RPAR, // )
    TK_LBR, // {
    TK_RBR, // }
    TK_LSQUARE, // [
    TK_RSQUARE, // ]
};

struct token {
    enum token_type type;
    char* literal;
    int cap, line, col;
};

struct token_list {
    unsigned int size, cap;
    struct token **tokens;
};
struct token_list *new_token_list(unsigned int init_cap);
struct token *token_list_get(struct token_list *list, unsigned int i);
int token_list_add(struct token_list *list, struct token *tk);
int token_list_expand(struct token_list *list);
int token_list_clean(struct token_list *list);

#endif
