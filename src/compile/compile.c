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

void compile_init(Compiler *compiler, Chunk *main)
{
    compiler->main = main;
    compiler->functions = malloc(sizeof(FunctionSymbol) * 32);
    compiler->function_count = 0;
    compiler->function_capacity = 32;
    compiler->current_function = NULL;
}

void compile(Compiler *compiler, ASTNode *node, Chunk *chunk)
{
    if (node == NULL)
        return;

    switch (node->type)
    {

    case NODE_FUNCTION:
    {
        FunctionNode *function_node = (FunctionNode *)node;
        FunctionSymbol *function_symbol = malloc(sizeof(FunctionSymbol));

        function_symbol->name = malloc(sizeof(char) * (strlen(function_node->name) + 1));
        strcpy(function_symbol->name, function_node->name);

        function_symbol->params_names = function_node->params;
        function_symbol->parameter_count = function_node->parameter_count;

        function_symbol->chunk = malloc(sizeof(Chunk));
        chunk_init(function_symbol->chunk);

        compiler->functions[compiler->function_count++] = function_symbol;

        FunctionSymbol *previous_function = compiler->current_function;
        compiler->current_function = function_symbol;

        for (size_t i = 0; i < function_node->body->statement_count; i++)
        {
            compile(compiler, function_node->body->statements[i], function_symbol->chunk);
        }

        emit_byte(function_symbol->chunk, OP_RETURN);
        compiler->current_function = previous_function;
        break;
    }

    case NODE_CALL_FUNCTION:
    {
        CallFunctionNode *call_node = (CallFunctionNode *)node;
        size_t function_index = 0;

        while (function_index < compiler->function_count &&
               strcmp(compiler->functions[function_index]->name,
                      call_node->name) != 0)
        {
            function_index++;
        }

        if (function_index == compiler->function_count)
        {
            fprintf(stderr, "Função '%s' não declarada.\n", call_node->name);
            break;
        }

        FunctionSymbol *function = compiler->functions[function_index];

        if (call_node->argument_count != function->parameter_count)
        {
            fprintf(stderr, "Quantidade incorreta de argumentos para '%s'.\n", call_node->name);
            break;
        }

        for (size_t i = 0; i < call_node->argument_count; i++)
        {
            compile(compiler, call_node->arguments[i], chunk);
        }

        emit_byte(chunk, OP_CALL_FUNCTION);
        emit_byte(chunk, (unsigned char)function_index);
        break;
    }

    case NODE_INDENTIFIER:
    {
        IdentifierNode *identifier = (IdentifierNode *)node;

        if (compiler->current_function == NULL)
        {
            fprintf(stderr, "Parâmetro '%s' usado fora de uma função.\n",
                    identifier->name);
            exit(0);
            break;
        }

        size_t index = 0;
        while (index < compiler->current_function->parameter_count &&
               strcmp(compiler->current_function->params_names[index],
                      identifier->name) != 0)
        {
            index++;
        }

        if (index == compiler->current_function->parameter_count)
        {
            fprintf(stderr, "Parâmetro '%s' não declarado.\n", identifier->name);
            exit(0);
            break;
        }

        emit_byte(chunk, OP_GET_PARAMETER);
        emit_byte(chunk, (unsigned char)index);
        break;
    }

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

        compile(compiler, o->left, chunk);
        compile(compiler, o->right, chunk);

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

        compile(compiler, p->expression, chunk);
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

    case NODE_MOD:
    {
        emit_byte(chunk, OP_MOD_CMD);
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
