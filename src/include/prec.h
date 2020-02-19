#ifndef _M_PREC_H__
#define _M_PREC_H__

enum prec {
    P_LOWEST,
    P_ASSIGN,
    P_LOR,
    P_LAND,
    P_CMP,
    P_BIT_OP,
    P_SHIFT,
    P_SUM,
    P_PRODUCT,
    P_UNARY,
    P_CALL,
    P_GROUP,
};

const char *get_prec_name(enum prec p);

#endif
