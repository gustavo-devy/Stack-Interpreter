#ifndef ERROR_H
#define ERROR_H

#include <stddef.h>

typedef struct TokenLocation
{
    const char *filename;
    size_t start;
    size_t end;
    size_t line;
    size_t column;
} TokenLocation;

typedef struct SourceLocation
{
    const char *filename;
    const char *source;
    size_t line;
    size_t column;
    size_t offset;
    size_t length;
} SourceLocation;

#endif // ERROR_H
