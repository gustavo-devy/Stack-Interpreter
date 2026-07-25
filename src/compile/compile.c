#include "compile/compile.h"

static void emit_byte(Chunk *chunk, unsigned char byte)
{
    if (chunk->count + 1 > chunk->capacity)
    {
        chunk->capacity = chunk->capacity < 8 ? 8 : chunk->capacity * 2;
        chunk->code = realloc(chunk->code, chunk->capacity * sizeof(unsigned char));
    }
    chunk->code[chunk->count++] = byte;
}

static int add_constant(Chunk *chunk, int value)
{
    if (chunk->constant_count + 1 > chunk->constant_capacity)
    {
        chunk->constant_capacity = chunk->constant_capacity < 8 ? 8 : chunk->constant_capacity * 2;
        chunk->constants = realloc(chunk->constants, chunk->constant_capacity * sizeof(int));
    }
    chunk->constants[chunk->constant_count] = value;
    return chunk->constant_count++;
}

void chunk_init(Chunk *chunk)
{
    chunk->code = malloc(sizeof(unsigned char) * 256);
    chunk->count = 0;
    chunk->capacity = 256;
    chunk->constants = malloc(sizeof(int) * 256);
    chunk->constant_count = 0;
    chunk->constant_capacity = 256;
}

void compile(ASTNode *node, Chunk *chunk)
{
    if (node == NULL)
        return;

    switch (node->type)
    {

    case NODE_NUMBER:
    {
        NumberNode *i = (NumberNode *)node;
        int constant_index = add_constant(chunk, i->value);

        emit_byte(chunk, OP_CONSTANT);
        emit_byte(chunk, (unsigned char)constant_index);
        break;
    }

    case NODE_OP:
    {
        OpNode *o = (OpNode *)node;

        compile(o->left, chunk);
        compile(o->right, chunk);

        if (o->op == '+')
        {
            emit_byte(chunk, OP_ADD);
        }
        else if (o->op == '-')
        {
            emit_byte(chunk, OP_SUBTRACT);
        }
        else if (o->op == '*')
        {
            emit_byte(chunk, OP_MULTIPLICATION);
        }
        else if (o->op == '/')
        {
            emit_byte(chunk, OP_DIVISION);
        }
        break;
    }

    case NODE_PUSH:
    {
        PushNode *p = (PushNode *)node;

        if (p->expression == NULL)
        {
            printf("Erro: push sem expressão.\n");
            break;
        }

        compile(p->expression, chunk);
        emit_byte(chunk, OP_PUSH_CMD);
        break;
    }

    case NODE_POP:
    {
        emit_byte(chunk, OP_POP_CMD);
        break;
    }

    case NODE_ADD:
    {
        emit_byte(chunk, OP_ADD_CMD);
        break;
    }

    case NODE_SUB:
    {
        emit_byte(chunk, OP_SUB_CMD);
        break;
    }

    case NODE_MUL:
    {
        emit_byte(chunk, OP_MUL_CMD);
        break;
    }

    case NODE_DIV:
    {
        emit_byte(chunk, OP_DIV_CMD);
        break;
    }

    case NODE_PRINT:
    {
        emit_byte(chunk, OP_PRINT_CMD);
        break;
    }

    case NODE_END:
    {
        emit_byte(chunk, OP_HALT);
        break;
    }

    default:
        printf("Erro de compilação!");
    }
}
