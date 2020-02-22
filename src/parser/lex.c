#include "../include/m.h"

static void fill(struct lexer *lex);
static void retarget_buf(struct lexer *lex);
static int ensure_token_space(struct token *tk, int i);

struct lexer *new_lexer() {
    struct lexer *lex;
    lex = (struct lexer *) malloc (sizeof(struct lexer));
    CHECK_NULL(lex);
    return lex;
}

int init_lexer(struct lexer *lex, const char *filename, const char *str) {
    if (filename && strlen(filename) > 0) {
        m_strdup(&lex->filename, filename);
        lex->fp = fopen(filename, "rb");
        CHECK_NULL_ERROR(lex->fp);
        fill(lex);
        retarget_buf(lex);
    } else if (str && strlen(str) > 0) {
        m_strdup(&lex->str, str);
        lex->cur = lex->str;
        lex->end = lex->str + strlen(str);
    }

    lex->filled = lex->idx = 0;
    lex->line = lex->col = 1;
    RETURN_OK;
}

static void fill(struct lexer *lex) {
    if (lex->fp == NULL || lex->filled) {
        return;
    }

    int idx = 1 - lex->idx;
    memset(lex->buf[idx], 0, LEX_BUF_LEN);
    fgets(lex->buf[idx], LEX_BUF_LEN, lex->fp);
    lex->filled = 1;
}

static void retarget_buf(struct lexer *lex) {
    if (!lex->filled) {
        return;
    }

    int idx = 1 - lex->idx;
    lex->idx = idx;
    lex->cur = lex->buf[idx];
    lex->end = lex->buf[idx] + strlen(lex->buf[idx]);
    lex->filled = 0;
}

char next(struct lexer *lex) {
    char ch = M_EOF;
    if (lex->cur >= lex->end) {
        fill(lex);
        retarget_buf(lex);
    }
    if (lex->cur < lex->end) {
        ch = *lex->cur;
        lex->cur++;
        lex->col++;
    }
    return ch;
}

char peek(struct lexer *lex) {
    if (lex->cur < lex->end) {
        return *lex->cur;
    }

    fill(lex);
    int idx = 1 - lex->idx;
    if (strlen(lex->buf[idx]) == 0) {
        return M_EOF;
    }
    return lex->buf[idx][0];
}

void skip(struct lexer *lex) {
    char ch = next(lex);
    while (WHITESPACE(ch)) {
        ch = next(lex);
    }
    if (ch != M_EOF) {
        back(lex);
    }
}

void back(struct lexer *lex) {
    lex->cur--;
    lex->col--;
}

char *get_token_info(const struct token *tk) {
    char *buf = (char *) malloc (64);
    memset(buf, 0, 64);
    if (tk) {
        sprintf(buf, "token: %s, literal: %s, line: %d, col: %d",
                token_expr(tk->type), tk->literal ? tk->literal : "NULL",
                tk->line, tk->col);
    } else {
        sprintf(buf, "NULL");
    }
    return buf;
}

struct token *tokenizer(struct lexer *lex) {
    struct token *t;
    int i, dot, exp;
    char prev_ch, ch;

    t = malloc (sizeof(struct token));
    t->type = TK_INVALID;
    t->literal = (char *) malloc (INIT_TOKEN_LEN);
    CHECK_NULL(t->literal);
    memset(t->literal, 0, INIT_TOKEN_LEN);
    t->cap = INIT_TOKEN_LEN;
    //printf("tokenizer current buf: %s\n", lex->cur);
    skip(lex);

    i = dot = exp = 0;
    prev_ch = 0;
    t->line = lex->line;
    t->col = lex->col;
    ch = next(lex);
    if (ch == M_EOF) {
        t->type = TK_EOF;
    } else if (ch == '\'') {
        ch = next(lex);
        if (ch == '\\') {
            ch = next(lex);
            switch (ch) {
                case 't':
                    t->literal[i++] = '\t';
                    break;
                case 'n':
                    t->literal[i++] = '\n';
                    break;
                case 'r':
                    t->literal[i++] = '\r';
                    break;
                case 'b':
                    t->literal[i++] = '\b';
                    break;
                case '\\':
                    t->literal[i++] = '\\';
                    break;
                case '\'':
                    t->literal[i++] = '\'';
                    break;
                default:
                    t->type = TK_INVALID;
                    goto out;
            }
        } else {
            t->literal[i++] = ch;
        }
        ch = next(lex);
        if (ch == '\'') t->type = TK_CHAR;
    } else if (ch == '#') {
        t->type = TK_COMMENT;
        ch = next(lex);
        while (ch != '\n') {
            t->literal[i++] = ch;
            ch = next(lex);
        }
        lex->line++;
        lex->col = 1;
    } else if (ch == '.') {
        t->type = TK_DOT;
        t->literal[i++] = ch;
        ch = peek(lex);
        if (DIGIT(ch)) {
            goto fraction;
        }
    } else if (DIGIT(ch)) {
        t->type = TK_INT;
        if (ch == '0') {
            t->literal[i++] = ch;
            ch = next(lex);
            if (ch == 'x' || ch == 'X') {
                // 0x[0-9a-fA-F]
                t->literal[i++] = ch;
                ch = next(lex);
                while (DIGIT(ch) || ('a' <= ch && ch <= 'f') || ('A' <= ch && ch <= 'Z')) {
                    if (ensure_token_space(t, i) == ERROR) {
                        goto token_space_error;
                    }
                    t->literal[i++] = ch;
                    ch = next(lex);
                }
                if (strlen(t->literal) == 2) {
                    t->type = TK_INVALID;
                    goto number_out;
                }
            } else if (ch == '.') {
                goto fraction;
            } else if (DIGIT(ch)) {
                // 0[0-9]*
                t->type = TK_INVALID;
                goto number_out;
            }
        }

digit_series:
        while (DIGIT(ch)) {
            if (ensure_token_space(t, i) == ERROR) {
                goto token_space_error;
            }
            t->literal[i++] = ch;
            ch = next(lex);
        }

        if (ch == '.') {
fraction:
            if (dot > 0 || exp > 0) {
                t->type = TK_INVALID;
                goto number_out;
            }
            dot = 1;
            if (ensure_token_space(t, i) == ERROR) {
                goto token_space_error;
            }
            t->literal[i++] = ch;
            t->type = TK_FLOAT;
            ch = next(lex);
            goto digit_series;
        }

        if (ch == 'e' || ch == 'E') {
exponent:
            exp = 1;
            if (ensure_token_space(t, i) == ERROR) {
                goto token_space_error;
            }
            t->literal[i++] = ch;
            ch = next(lex);
            if (DIGIT(ch)) {
                goto digit_series;
            } else if (ch == '-') {
                if (ensure_token_space(t, i) == ERROR) {
                    goto token_space_error;
                }
                t->literal[i++] = ch;
                t->type = TK_FLOAT;
                ch = next(lex);
                if (DIGIT(ch)) {
                    goto digit_series;
                } else {
                    t->type = TK_INVALID;
                    goto number_out;
                }
            } else {
                t->type = TK_INVALID;
                goto number_out;
            }
        }

number_out:
        if (ch != M_EOF) {
            back(lex);
        }
    } else if (ALPHANUM(ch)) {
        while (ALPHANUM(ch)) {
            if (ensure_token_space(t, i) == ERROR) {
                goto token_space_error;
            }
            t->literal[i++] = ch;
            ch = next(lex);
        }
        t->literal[i] = 0;
        t->type = find_keyword(t->literal);
        if (ch != M_EOF) {
            back(lex);
        }
    } else if (ch == '"') {
        ch = next(lex);
        while (ch != '"' || prev_ch == '\\') {
            if (ensure_token_space(t, i) == ERROR) {
                goto token_space_error;
            }
            t->literal[i++] = ch;
            prev_ch = ch;
            ch = next(lex);
        }
        if (ch == '"') {
            t->type = TK_STRING;
        }
    } else if (ch == '=') {
        t->literal[i++] = ch;
        t->type = TK_ASSIGN;
        if ((ch = peek(lex)) == '=') {
            t->type = TK_EQ;
            t->literal[i++] = ch;
            next(lex);
        }
    } else if (ch == '<') {
        t->literal[i++] = ch;
        t->type = TK_LT;
        ch = peek(lex);
        if (ch == '=') {
            t->type = TK_LE;
            t->literal[i++] = ch;
            next(lex);
        } else if (ch == '<') {
            t->type = TK_LSHIFT;
            t->literal[i++] = ch;
            next(lex);
            ch = peek(lex);
            if (ch == '=') {
                t->type = TK_LSHIFT_ASSIGN;
                t->literal[i++] = ch;
                next(lex);
            }
        }
    } else if (ch == '>') {
        t->literal[i++] = ch;
        t->type = TK_GT;
        ch = next(lex);
        if (ch == '=') {
            t->type = TK_GE;
            t->literal[i++] = ch;
            next(lex);
        } else if (ch == '>') {
            t->type = TK_RSHIFT;
            t->literal[i++] = ch;
            next(lex);
            ch = peek(lex);
            if (ch == '=') {
                t->type = TK_RSHIFT_ASSIGN;
                t->literal[i++] = ch;
                next(lex);
            }
        }
    } else if (ch == '!') {
        t->literal[i++] = ch;
        t->type = TK_NOT;
        if ((ch = peek(lex)) == '=') {
            t->type = TK_NE;
            t->literal[i++] = ch;
            next(lex);
        }
    } else if (ch == '&') {
        t->literal[i++] = ch;
        t->type = TK_AND;
        ch = peek(lex);
        if (ch == '&') {
            t->type = TK_LAND;
            t->literal[i++] = ch;
            next(lex);
        } else if (ch == '=') {
            t->type = TK_AND_ASSIGN;
            t->literal[i++] = ch;
            next(lex);
        }
    } else if (ch == '|') {
        t->literal[i++] = ch;
        t->type = TK_OR;
        ch = peek(lex);
        if (ch == '|') {
            t->type = TK_LOR;
            t->literal[i++] = ch;
            next(lex);
        } else if (ch == '=') {
            t->type = TK_OR_ASSIGN;
            t->literal[i++] = ch;
            next(lex);
        }
    } else if (ch == '+') {
        t->literal[i++] = ch;
        t->type = TK_PLUS;
        if ((ch = peek(lex)) == '=') {
            t->type = TK_PLUS_ASSIGN;
            t->literal[i++] = ch;
            next(lex);
        }
    } else if (ch == '-') {
        t->literal[i++] = ch;
        t->type = TK_MINUS;
        if ((ch = peek(lex)) == '=') {
            t->type = TK_MINUS_ASSIGN;
            t->literal[i++] = ch;
            next(lex);
        }
    } else if (ch == '*') {
        t->literal[i++] = ch;
        t->type = TK_STAR;
        if ((ch = peek(lex)) == '=') {
            t->type = TK_STAR_ASSIGN;
            t->literal[i++] = ch;
            next(lex);
        }
    } else if (ch == '/') {
        t->literal[i++] = ch;
        t->type = TK_SLASH;
        if ((ch = peek(lex)) == '=') {
            t->type = TK_SLASH_ASSIGN;
            t->literal[i++] = ch;
            next(lex);
        }
    } else if (ch == '%') {
        t->literal[i++] = ch;
        t->type = TK_PERCENT;
        if ((ch = peek(lex)) == '=') {
            t->type = TK_PERCENT_ASSIGN;
            t->literal[i++] = ch;
            next(lex);
        }
    } else if (ch == '^') {
        t->literal[i++] = ch;
        t->type = TK_XOR;
        if ((ch = peek(lex)) == '=') {
            t->type = TK_XOR_ASSIGN;
            t->literal[i++] = ch;
            next(lex);
        }
    } else if (ch == '~') {
        t->literal[i++] = ch;
        t->type = TK_NEGATE;
    } else if (ch == ',') {
        t->literal[i++] = ch;
        t->type = TK_COMMA;
    } else if (ch == ';') {
        t->literal[i++] = ch;
        t->type = TK_SEMICOLON;
    } else if (ch == ':') {
        t->literal[i++] = ch;
        t->type = TK_COLON;
    } else if (ch == '(') {
        t->literal[i++] = ch;
        t->type = TK_LPAR;
    } else if (ch == ')') {
        t->literal[i++] = ch;
        t->type = TK_RPAR;
    } else if (ch == '{') {
        t->literal[i++] = ch;
        t->type = TK_LBR;
    } else if (ch == '}') {
        t->literal[i++] = ch;
        t->type = TK_RBR;
    } else if (ch == '[') {
        t->literal[i++] = ch;
        t->type = TK_LSQUARE;
    } else if (ch == ']') {
        t->literal[i++] = ch;
        t->type = TK_RSQUARE;
    } else if (ch == '\n') {
        t->literal[i++] = ch;
        t->type = TK_NEWLINE;
        lex->line++;
        lex->col = 1;
    }
    goto out;

token_space_error:
    t->type = TK_INVALID;
out:
    return t;
}

static int ensure_token_space(struct token *tk, int i) {
    if (i - 1 < tk->cap) {
        RETURN_OK;
    }
    if (tk->cap >= MAX_TOKEN_LEN && tk->type != TK_STRING) {
        RETURN_ERROR;
    }

    int cap = tk->cap + tk->cap / 2;
    if (tk->type != TK_STRING) {
        if (cap >= MAX_TOKEN_LEN && tk->cap < MAX_TOKEN_LEN) {
            cap = MAX_TOKEN_LEN;
        }
    }
    tk->literal = (char *) realloc (tk->literal, cap);
    CHECK_NULL_ERROR(tk->literal);
    tk->cap = cap;
    RETURN_OK;
}

static const char *kw[] = {
    "var",
    "val",
    "if",
    "elif",
    "else",
    "for",
    "func",
    "return",
    "true",
    "false",
    "nil",
};

int find_keyword(const char *ident) {
    int i;
    static int len = 0;
    if (len == 0) {
        len = sizeof(kw) / sizeof(kw[0]);
    }

    for (i = 0; i < len; ++i) {
        if (strcmp(kw[i], ident) == 0) {
            return i+1+TK_IDENT;
        }
    }
    return TK_IDENT;
}

static const char *token_str[] = {
    "TK_INVALID \"invalid token\"",
    "TK_COMMENT \"#\"",
    "TK_EOF \"end of file\"",
    "TK_NEWLINE \"new line\"",

    "TK_CHAR \"character literal\"",
    "TK_INT \"int literal\"",
    "TK_FLOAT \"float literal\"",
    "TK_STRING \"string literal\"",
    "TK_IDENT \"identifier\"",

    "TK_VAR \"var\"", // var
    "TK_VAL \"val\"", // val
    "TK_IF \"if\"", // if
    "TK_ELSEIF \"elif\"", // elif
    "TK_ELSE \"else\"", // else
    "TK_FOR \"for\"", // for
    "TK_FUNC \"func\"", // func
    "TK_RET \"return\"", // return
    "TK_TRUE \"true\"", // true
    "TK_FALSE \"false\"", // false
    "TK_NIL \"nil\"", // nil

    "TK_SEMICOLON \";\"", // ;
    "TK_COLON \":\"", // :
    "TK_COMMA \",\"", // ,
    "TK_DOT \".\"", // ,

    "TK_ASSIGN \"=\"", // =
    "TK_EQ \"==\"", // ==
    "TK_NOT \"!\"", // !
    "TK_NE \"!=\"", // !=
    "TK_LT \"<\"", // <
    "TK_LSHIFT \"<<\"", // <<
    "TK_LSHIFT_ASSIGN \"<<=\"", // <<=
    "TK_LE \"<=\"", // <=
    "TK_GT \">\"", // >
    "TK_RSHIFT \">>\"", // >>
    "TK_RSHIFT_ASSIGN \">>=\"", // >>=
    "TK_GE \">=\"", // >=
    "TK_AND \"&\"", // &
    "TK_AND_ASSIGN \"&=\"", // &=
    "TK_OR \"|\"", // |
    "TK_OR_ASSIGN \"|=\"", // |=
    "TK_XOR \"^\"", // ^
    "TK_XOR_ASSIGN \"^=\"", // ^=
    "TK_PLUS \"+\"", // +
    "TK_PLUS_ASSIGN \"+=\"", // +=
    "TK_MINUS \"-\"", // -
    "TK_MINUS_ASSIGN \"-=\"", // -=
    "TK_STAR \"*\"", // *
    "TK_STAR_ASSIGN \"*=\"", // *=
    "TK_SLASH \"/\"", // /
    "TK_SLASH_ASSIGN \"/=\"", // /=
    "TK_PERCENT \"%\"", // %
    "TK_PERCENT_ASSIGN \"%=\"", // %=
    "TK_NEGATE \"~\"", // ~
    "TK_LAND \"&&\"", // &&
    "TK_LOR \"||\"", // ||
    "TK_LPAR \"(\"", // (
    "TK_RPAR \")\"", // )
    "TK_LBR \"{\"", // {
    "TK_RBR \"}\"", // }
    "TK_LSQUARE \"[\"", // [
    "TK_RSQUARE \"]\"", // ]
};

const char *token_expr(int type) {
    static int len = sizeof(token_str) / sizeof(token_str[0]);
    if (type >= len) {
        type = 0;
    }
    return token_str[type];
}
