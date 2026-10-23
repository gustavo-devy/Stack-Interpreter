#ifndef COMPILE_H
#define COMPILE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "parser/parser.h"

typedef enum OpCode
{
    OP_CONSTANT,
    OP_GET_VAR,
    OP_GET_PARAMETER,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLICATION,
    OP_DIVISION,
    OP_PUSH_CMD,
    OP_POP_CMD,
    OP_ADD_CMD,
    OP_SUB_CMD,
    OP_MUL_CMD,
    OP_DIV_CMD,
    OP_MOD_CMD,
    OP_PRINT_CMD,
    OP_CALL_FUNCTION,
    OP_RETURN,
    OP_HALT
} OpCode;

typedef struct Chunk
{
    unsigned char *code;
    int count;
    int capacity;
    int *constants;
    int constant_count;
    int constant_capacity;
} Chunk;

typedef struct FunctionSymbol
{
    char *name;
    Chunk *chunk;
    char **params_names;
    size_t parameter_count;
} FunctionSymbol;

typedef struct Compiler
{
    Chunk *main;
    FunctionSymbol **functions;
    FunctionSymbol *current_function;
    size_t function_count;
    size_t function_capacity;
} Compiler;

void chunk_init(Chunk *chunk);
void compile_init(Compiler *compiler, Chunk *main);
void compile(Compiler *compiler, ASTNode *node, Chunk *chunk);

#endif // COMPILE_H
