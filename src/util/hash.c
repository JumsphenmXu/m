#include "../include/m.h"

unsigned int fnv32(char *data, int size) {
    unsigned int h;
    char *s, *e;

    s = data;
    e = data + size;
    h = 0;
    while (s < e) {
        h ^= (unsigned int) *s++;
        h += (h<<1) + (h<<4) + (h<<7) + (h<<8) + (h<<24);
    }

    return h;
}

unsigned long fnv64(char *data, int size) {
    unsigned long h;
    char *s, *e;

    s = data;
    e = data + size;
    h = 0;
    while (s < e) {
        h ^= (unsigned int) *s++;
        h += (h<<1) + (h<<4) + (h<<5) + (h<<7) + (h<<8) + (h<<40);
    }

    return h;
}
