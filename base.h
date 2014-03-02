#ifndef BASE_INCLUDE_H
#define BASE_INCLUDE_H

#define MALLOC  malloc
#define FREE  free
#define NEW(p) {p = MALLOC(sizeof(*p));}

#define PRINT(fmt, arg...) printf(fmt, ##arg)

#endif
