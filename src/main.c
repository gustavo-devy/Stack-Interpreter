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
        printf("%s\n", ops[(int)op]);
    }
    printf("\n");
}

void lexer_view_tokens(Lexer lexer)
{
    Token *token = lexer_next_token(&lexer);
    while (token->type != TOKEN_END)
    {
        printf("TYPE: [%d], VALUE: %s, LINE: %lu, COLUMN: %lu, START: %lu, END: %lu\n",
               token->type, token->value, token->location.line, token->location.column, token->location.start, token->location.end);
        token = lexer_next_token(&lexer);
    }
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

    DiagnosticList *diags = malloc(sizeof(DiagnosticList));
    diag_list_init(diags);

    Lexer lexer;
    lexer_init(&lexer, argv[1], input, diags);

    Parser parser;
    parser_init(&parser, diags);

    ASTNode **program = NULL;
    size_t program_count = 0;
    size_t program_capacity = 0;

    ASTNode *node = NULL;
    while (1)
    {
        node = parser_code(&parser, &lexer);
        if (node == NULL)
            break;

        if (program_count + 1 > program_capacity)
        {
            program_capacity = program_capacity < 8 ? 8 : program_capacity * 2;
            program = realloc(program, program_capacity * sizeof(ASTNode *));
        }

        program[program_count++] = node;

        if (node->type == NODE_END)
            break;
    }

    Chunk chunk;
    chunk_init(&chunk);

    Compiler compiler;
    compile_init(&compiler, &chunk);

    for (size_t i = 0; i < program_count; i++)
    {
        compile(&compiler, program[i], &chunk, diags);
    }

    if (node == NULL || diags->count > 0)
    {
        for (size_t i = 0; i < diags->count; i++)
        {
            diag_render(&diags->items[i], stderr);
        }

        for (size_t i = 0; i < program_count; i++)
        {
            parser_free_ast(program[i]);
        }

        if (parser.current_token != NULL)
            lexer_free_token(parser.current_token);

        free(program);
        free(diags);
        free(input);
        return EXIT_FAILURE;
    }

    VM vm;
    vm_init(&vm, &chunk, &compiler);
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
    free(diags);

    return EXIT_SUCCESS;
}
