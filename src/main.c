#include <stdio.h>
#include <stdlib.h>

#include "file/file.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "compile/compile.h"
#include "vm/vm.h"

char *ops[15] = {
    "OP_CONSTANT",
    "OP_GET_VAR",
    "OP_ADD",
    "OP_SUBTRACT",
    "OP_MULTIPLICATION",
    "OP_DIVISION",
    "OP_PUSH_CMD",
    "OP_POP_CMD",
    "OP_ADD_CMD",
    "OP_SUB_CMD",
    "OP_MUL_CMD",
    "OP_DIV_CMD",
    "OP_MOD_CMD",
    "OP_PRINT_CMD",
    "OP_HALT" //
};

void debug(Chunk chunk)
{
    for (int i = 0; i < chunk.count; i++)
    {
        OpCode op = chunk.code[i];
        if (op == OP_CONSTANT)
        {
            i++;
        }
        printf("%s\n", ops[(int)op]);
    }
    printf("\n");
}

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Uso: %s <arquivo>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *input = read_file(argv[1]);
    if (input == NULL)
    {
        fprintf(stderr, "Erro ao ler o arquivo: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    Lexer lexer;
    lexer_init(&lexer, input);

    Parser parser;
    parser_init(&parser);

    ASTNode **program = NULL;
    size_t program_count = 0;
    size_t program_capacity = 0;

    ASTNode *node = parser_code(&parser, &lexer);
    while (node != NULL && node->type != NODE_END)
    {
        if (program_count + 1 > program_capacity)
        {
            program_capacity = program_capacity < 8 ? 8 : program_capacity * 2;
            program = realloc(program, program_capacity * sizeof(ASTNode *));
        }

        program[program_count++] = node;
        node = parser_code(&parser, &lexer);
    }

    if (node != NULL)
    {
        if (program_count + 1 > program_capacity)
        {
            program_capacity = program_capacity < 8 ? 8 : program_capacity * 2;
            program = realloc(program, program_capacity * sizeof(ASTNode *));
        }

        program[program_count++] = node;
    }

    Chunk chunk;
    chunk_init(&chunk);

    for (size_t i = 0; i < program_count; i++)
    {
        compile(program[i], &chunk);
    }

    debug(chunk);

    VM vm;
    vm_init(&vm, &chunk);
    run_vm(&vm);

    free(chunk.code);
    free(chunk.constants);

    for (size_t i = 0; i < program_count; i++)
    {
        if (program[i] == NULL)
            continue;
        parser_free_ast(program[i]);
    }

    if (parser.current_token != NULL)
    {
        lexer_free_token(parser.current_token);
        parser.current_token = NULL;
    }

    free(program);
    free(input);

    return EXIT_SUCCESS;
}
