#ifndef COMPILE_H
#define COMPILE_H

#include <stdio.h>
#include <stdlib.h>
#include "parser/parser.h"

typedef enum
{
    OP_CONSTANT,
    OP_GET_VAR,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLICATION,
    OP_DIVISION,
    OP_PUSH_CMD,
    OP_POP_CMD,
    OP_PRINT_CMD,
    OP_HALT
} OpCode;

typedef struct
{
    unsigned char *code;
    int count;
    int capacity;
    int *constants;
    int constant_count;
    int constant_capacity;
} Chunk;

void chunk_init(Chunk *chunk);
void compile(ASTNode *node, Chunk *chunk);

#endif // COMPILE_H
