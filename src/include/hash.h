#ifndef _M_HASH_H__
#define _M_HASH_H__

#define FNV32_PRIME (16777619)
#define FNV64_PRIME (1099511628211)

unsigned int fnv32(char *data, int size);
unsigned long fnv64(char *data, int size);

#endif
