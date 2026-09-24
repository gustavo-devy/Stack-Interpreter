#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "parser/parser.h"

typedef enum Precedence
{
    PREC_NONE,
    PREC_ASSIGNMENT, // =
    PREC_TERM,       // + e -
    PREC_FACTOR,     // * e /
    PREC_PRIMARY     // Números, Variáveis e Parênteses ()
} Precedence;

typedef ASTNode *(*PrefixFn)(Parser *parser, Lexer *lexer);
typedef ASTNode *(*InfixFn)(Parser *parser, ASTNode *left, Lexer *lexer);

typedef struct ParseRule
{
    PrefixFn prefix;
    InfixFn infix;
    Precedence precedence;
} ParseRule;

static Token *lexer_peek_token(Lexer *lexer)
{
    Lexer copy = *lexer;
    return lexer_next_token(&copy);
}

static void parser_advance_token(Parser *parser, Lexer *lexer)
{
    if (parser->current_token != NULL)
    {
        lexer_free_token(parser->current_token);
    }

    parser->current_token = lexer_next_token(lexer);
    parser->position++;
}

static NumberNode *create_number_node(int64_t value)
{
    NumberNode *node = malloc(sizeof(NumberNode));
    node->base.type = NODE_NUMBER;
    node->value = value;
    return node;
}

static ASTNode *parse_number(Parser *parser, Lexer *lexer)
{
    (void)lexer;

    Token *token = parser->current_token;
    int64_t value = atoll(token->value);
    return (ASTNode *)create_number_node(value);
}

static ParseRule *get_rule(TokenType type);
static ASTNode *parse_expression(Parser *parser, Lexer *lexer, Precedence precedence);

static ASTNode *parse_grouping(Parser *parser, Lexer *lexer)
{
    parser_advance_token(parser, lexer);
    ASTNode *expression = parse_expression(parser, lexer, PREC_NONE);

    if (parser->current_token->type != TOKEN_RIGHT_PAREN)
    {
        printf("Erro: parêntese direito esperado.\n");
        return NULL;
    }

    parser_advance_token(parser, lexer);
    return expression;
}

static ASTNode *parse_binary(Parser *parser, ASTNode *left, Lexer *lexer)
{
    char operator = parser->current_token->value[0];
    ParseRule *rule = get_rule(parser->current_token->type);
    Precedence precedence = rule->precedence;

    parser_advance_token(parser, lexer);
    ASTNode *right = parse_expression(parser, lexer, precedence);

    OpNode *node = malloc(sizeof(OpNode));
    node->base.type = NODE_OP;
    node->op = operator;
    node->left = left;
    node->right = right;
    return (ASTNode *)node;
}

static ParseRule *get_rule(TokenType type)
{
    static ParseRule rules[TOKEN_END + 1] = {
        [TOKEN_RESERVED_WORD] = {NULL, NULL, PREC_NONE},
        [TOKEN_IDENTIFIER] = {NULL, NULL, PREC_NONE},
        [TOKEN_NUMBER] = {parse_number, NULL, PREC_NONE},
        [TOKEN_PLUS] = {NULL, parse_binary, PREC_TERM},
        [TOKEN_MINUS] = {NULL, parse_binary, PREC_TERM},
        [TOKEN_STAR] = {NULL, parse_binary, PREC_FACTOR},
        [TOKEN_SLASH] = {NULL, parse_binary, PREC_FACTOR},
        [TOKEN_LEFT_PAREN] = {parse_grouping, NULL, PREC_NONE},
        [TOKEN_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
        [TOKEN_END] = {NULL, NULL, PREC_NONE},
    };
    return &rules[type];
}

static ASTNode *parse_expression(Parser *parser, Lexer *lexer, Precedence precedence)
{
    PrefixFn prefix_rule = get_rule(parser->current_token->type)->prefix;
    if (prefix_rule == NULL)
    {
        printf("Erro: Esperava início de expressão.\n");
        return NULL;
    }

    ASTNode *left = prefix_rule(parser, lexer);
    Token *peeked = lexer_peek_token(lexer);

    while (peeked != NULL && precedence < get_rule(peeked->type)->precedence)
    {
        lexer_free_token(peeked);
        parser_advance_token(parser, lexer);

        InfixFn infix_rule = get_rule(parser->current_token->type)->infix;
        if (infix_rule == NULL)
        {
            printf("Erro: Operador inesperado em expressão.\n");
            return left;
        }

        left = infix_rule(parser, left, lexer);
        peeked = lexer_peek_token(lexer);
    }

    if (peeked != NULL)
    {
        lexer_free_token(peeked);
    }

    return left;
}

static ASTNode *parse_push(Parser *parser, Lexer *lexer)
{
    parser_advance_token(parser, lexer);
    ASTNode *expression = parse_expression(parser, lexer, PREC_NONE);

    PushNode *node = malloc(sizeof(PushNode));
    node->base.type = NODE_PUSH;
    node->expression = expression;
    return (ASTNode *)node;
}

static ASTNode *parse_pop(Parser *parser)
{
    (void)parser;

    PopNode *node = malloc(sizeof(PopNode));
    node->base.type = NODE_POP;
    node->expression = NULL;
    return (ASTNode *)node;
}

static void *parser_add(Parser *parser)
{
    (void)parser;

    AddNode *node = malloc(sizeof(AddNode));
    node->base.type = NODE_ADD;
    return (ASTNode *)node;
}

static void *parser_sub(Parser *parser)
{
    (void)parser;

    SubNode *node = malloc(sizeof(SubNode));
    node->base.type = NODE_SUB;
    return (ASTNode *)node;
}

static void *parser_mul(Parser *parser)
{
    (void)parser;

    MulNode *node = malloc(sizeof(MulNode));
    node->base.type = NODE_MUL;
    return (ASTNode *)node;
}

static void *parser_div(Parser *parser)
{
    (void)parser;

    DivNode *node = malloc(sizeof(DivNode));
    node->base.type = NODE_DIV;
    return (ASTNode *)node;
}

static void *parser_mod(Parser *parser)
{
    (void)parser;

    ModNode *node = malloc(sizeof(ModNode));
    node->base.type = NODE_MOD;
    return (ASTNode *)node;
}

static ASTNode *parser_print(Parser *parser)
{
    (void)parser;

    PrintNode *node = malloc(sizeof(PrintNode));
    node->base.type = NODE_PRINT;
    return (ASTNode *)node;
}

void parser_init(Parser *parser)
{
    parser->position = 0;
    parser->current_token = NULL;
}

ASTNode *parser_code(Parser *parser, Lexer *lexer)
{
    if (parser->current_token != NULL)
    {
        lexer_free_token(parser->current_token);
        parser->current_token = NULL;
    }

    parser->position = 0;
    parser->current_token = lexer_next_token(lexer);

    if (parser->current_token->type == TOKEN_END)
    {
        EndNode *node = malloc(sizeof(EndNode));
        node->base.type = NODE_END;

        return (ASTNode *)node;
    }

    if (parser->current_token->type == TOKEN_RESERVED_WORD)
    {
        switch (parser->current_token->reserved_word)
        {
        case RESERVED_WORD_PUSH:
            return parse_push(parser, lexer);
        case RESERVED_WORD_POP:
            return parse_pop(parser);
        case RESERVED_WORD_PRINT:
            return parser_print(parser);
        case RESERVED_WORD_ADD:
            return parser_add(parser);
        case RESERVED_WORD_SUB:
            return parser_sub(parser);
        case RESERVED_WORD_MUL:
            return parser_mul(parser);
        case RESERVED_WORD_DIV:
            return parser_div(parser);
        case RESERVED_WORD_MOD:
            return parser_mod(parser);
        default:
            printf("Erro: palavra reservada desconhecida.\n");
            return NULL;
        }
    }

    return parse_expression(parser, lexer, PREC_NONE);
}

void parser_free_ast(ASTNode *node)
{
    if (node == NULL)
        return;

    switch (node->type)
    {
    case NODE_OP:
    {
        OpNode *op = (OpNode *)node;
        parser_free_ast(op->left);
        parser_free_ast(op->right);
        break;
    }
    case NODE_PUSH:
    {
        PushNode *push = (PushNode *)node;
        parser_free_ast(push->expression);
        break;
    }
    case NODE_POP:
    {
        PopNode *pop = (PopNode *)node;
        parser_free_ast(pop->expression);
        break;
    }
    case NODE_NUMBER:
    case NODE_PRINT:
    case NODE_END:
    default:
        break;
    }

    free(node);
}
