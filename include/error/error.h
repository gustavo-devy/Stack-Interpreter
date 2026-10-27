#ifndef ERROR_H
#define ERROR_H

#include <stdint.h>

typedef struct TokenLocation
{
    const char *filename;
    size_t length;
    size_t start;
    size_t end;
    size_t line;
    size_t column;
    size_t offset;
} TokenLocation;

typedef struct SourceLocation
{
    const char *filename;
    const char *source;
    size_t line;
    size_t column;
    size_t offset;
} SourceLocation;

#endif // ERROR_H
