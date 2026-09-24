#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <stdint.h>
#include "lexer/lexer.h"

typedef enum NodeType
{
    NODE_PUSH,
    NODE_POP,
    NODE_NUMBER,
    NODE_OP,
    NODE_ADD,
    NODE_SUB,
    NODE_MUL,
    NODE_DIV,
    NODE_MOD,
    NODE_PRINT,
    NODE_END
} NodeType;

typedef struct ASTNode
{
    NodeType type;
} ASTNode;

typedef struct EndNode
{
    ASTNode base;
} EndNode;

typedef struct NumberNode
{
    ASTNode base;
    int64_t value;
} NumberNode;

typedef struct OpNode
{
    ASTNode base;
    char op;
    ASTNode *left;
    ASTNode *right;
} OpNode;

typedef struct PushNode
{
    ASTNode base;
    ASTNode *expression;
} PushNode;

typedef struct PopNode
{
    ASTNode base;
    ASTNode *expression;
} PopNode;

typedef struct PrintNode
{
    ASTNode base;
} PrintNode;

typedef struct AddNode
{
    ASTNode base;
} AddNode;

typedef struct SubNode
{
    ASTNode base;
} SubNode;

typedef struct MulNode
{
    ASTNode base;
} MulNode;

typedef struct DivNode
{
    ASTNode base;
} DivNode;

typedef struct ModNode
{
    ASTNode base;
} ModNode;

typedef struct Parser
{
    size_t position;
    Token *current_token;
} Parser;

void parser_init(Parser *parser);
ASTNode *parser_code(Parser *parser, Lexer *lexer);
void parser_free_ast(ASTNode *node);

#endif // PARSER_H
