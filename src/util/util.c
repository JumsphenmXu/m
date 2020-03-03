#include "../include/m.h"

void m_strndup(char **dest, const char *src, size_t n) {
    *dest = (char *) malloc (n+1);
    memcpy(*dest, src, n);
    (*dest)[n] = 0;
}

void m_strdup(char **dest, const char *src) {
    int n = strlen(src);
    *dest = (char *) malloc (n + 1);
    memcpy(*dest, src, n);
    (*dest)[n] = 0;
}

static const char *node_name[] = {
    "N_INVALID_LWM", // low watermark
    "N_STMT_VAR",
    "N_STMT_RET",
    "N_STMT_BLOCK",
    "N_STMT_IF",
    "N_STMT_FOR",
    "N_STMT_EXPR",
    "N_EXPR_IDENT",
    "N_EXPR_LITERAL",
    "N_EXPR_ARRAY",
    "N_EXPR_MAP",
    "N_EXPR_UNARY",
    "N_EXPR_BINARY",
    "N_EXPR_GROUP",
    "N_EXPR_FUNC",
    "N_EXPR_CALL",
    "N_EXPR_INDEX",
    "N_INVALID_HWM", // high watermark
};

const char *get_node_name(enum node_type type) {
    if (type <= N_INVALID_LWM || type >= N_INVALID_HWM) {
        return "INVALID_NODE";
    }
    return node_name[type];
}

static const char *precedences[] = {
    "P_LOWEST",
    "P_ASSIGN",
    "P_LOR",
    "P_LAND",
    "P_CMP",
    "P_BIT_OP",
    "P_SHIFT",
    "P_SUM",
    "P_PRODUCT",
    "P_UNARY",
    "P_CALL",
    "P_GROUP",
};

const char *get_prec_name(enum prec p) {
    if (p < P_LOWEST || p > P_GROUP) {
        return "INVALID_PREC";
    }
    return precedences[p];
}

static unsigned int map_capacity_primes[] = {
    7, 23, 41, 67, 89, 131, 193, 239, 277, 313, 367, 401, 467, 503,
};
unsigned int get_next_map_capacity(unsigned int cur) {
    unsigned int i, n, prime;

    n = sizeof(map_capacity_primes) / sizeof(unsigned int);
    prime  = map_capacity_primes[i];
    for (i = 0; i < n; ++i) {
        if (cur < map_capacity_primes[i]) {
            prime = map_capacity_primes[i];
            break;
        }
    }

    return prime;
}
