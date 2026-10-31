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
    DiagnosticList peek_diagnostics = {0};
    copy.diags = &peek_diagnostics;
    return lexer_next_token(&copy);
}

static SourceLocation parser_token_location(Lexer *lexer, Token *token)
{
    return (SourceLocation){
        .filename = token->location.filename,
        .source = lexer->source,
        .line = token->location.line,
        .column = token->location.column,
        .offset = token->location.start,
        .length = token->location.end - token->location.start,
    };
}

static void parser_report_unexpected_token(Parser *parser, Lexer *lexer,
                                           const char *code, const char *message)
{
    if (parser->current_token->type == TOKEN_UNKNOWN)
        return;

    diag_emit(parser->diags, SEV_ERROR, code,
              parser_token_location(lexer, parser->current_token), "%s", message);
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

    if (expression == NULL)
        return NULL;

    if (parser->current_token->type != TOKEN_RIGHT_PAREN)
    {
        parser_report_unexpected_token(parser, lexer, "E0101",
                                       "Expected ')' after expression");
        parser_free_ast(expression);
        return NULL;
    }

    parser_advance_token(parser, lexer);
    return expression;
}

static ASTNode *parse_binary(Parser *parser, ASTNode *left, Lexer *lexer)
{
    char operator = parser->current_token->value[0];

    ParseRule *rule = get_rule(parser->current_token->type);
    if (rule == NULL)
        return NULL;

    Precedence precedence = rule->precedence;

    parser_advance_token(parser, lexer);
    ASTNode *right = parse_expression(parser, lexer, precedence);

    if (right == NULL)
    {
        parser_free_ast(left);
        return NULL;
    }

    OpNode *node = malloc(sizeof(OpNode));
    node->base.type = NODE_OP;
    node->op = operator;
    node->left = left;
    node->right = right;
    return (ASTNode *)node;
}

static ASTNode *parse_indentifier(Parser *parser, Lexer *lexer)
{
    (void)lexer;

    IdentifierNode *node = malloc(sizeof(*node));
    node->base.type = NODE_INDENTIFIER;

    size_t length = strlen(parser->current_token->value) + 1;
    node->name = malloc(length);
    memcpy(node->name, parser->current_token->value, length);

    return (ASTNode *)node;
}

static ParseRule *get_rule(TokenType type)
{
    static ParseRule rules[TOKEN_END + 1] = {
        [TOKEN_IDENTIFIER] = {parse_indentifier, NULL, PREC_NONE},
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
        parser_report_unexpected_token(parser, lexer, "E0100",
                                       "Invalid expression start");
        return NULL;
    }

    ASTNode *left = prefix_rule(parser, lexer);
    if (left == NULL)
        return NULL;

    Token *peeked = lexer_peek_token(lexer);

    while (peeked != NULL && precedence < get_rule(peeked->type)->precedence)
    {
        lexer_free_token(peeked);
        parser_advance_token(parser, lexer);

        InfixFn infix_rule = get_rule(parser->current_token->type)->infix;
        if (infix_rule == NULL)
        {
            parser_report_unexpected_token(parser, lexer, "E0100",
                                           "Expected an operator in expression");
            parser_free_ast(left);
            return NULL;
        }

        left = infix_rule(parser, left, lexer);
        if (left == NULL)
            return NULL;

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

    if (expression == NULL)
        return NULL;

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

static ASTNode *parse_call_function(Parser *parser, Lexer *lexer)
{
    parser_advance_token(parser, lexer);
    if (parser->current_token->type != TOKEN_IDENTIFIER)
    {
        parser_report_unexpected_token(parser, lexer, "E0201",
                                      "Expected function name after 'call'");
        return NULL;
    }

    CallFunctionNode *node = malloc(sizeof(CallFunctionNode));
    node->base.type = NODE_CALL_FUNCTION;
    size_t name_length = strlen(parser->current_token->value) + 1;
    node->name = malloc(name_length);
    strcpy(node->name, parser->current_token->value);
    node->argument_count = 0;
    node->argument_capacity = 8;
    node->arguments = malloc(sizeof(ASTNode *) * 8);

    parser_advance_token(parser, lexer);

    if (parser->current_token->type != TOKEN_LEFT_ANGLE)
    {
        parser_report_unexpected_token(parser, lexer, "E0201",
                                      "Expected '<' before function arguments");
        parser_free_ast((ASTNode *)node);
        return NULL;
    }
    parser_advance_token(parser, lexer);

    while (parser->current_token->type != TOKEN_RIGHT_ANGLE &&
           parser->current_token->type != TOKEN_END)
    {
        ASTNode *argument_node = parse_expression(parser, lexer, PREC_NONE);
        if (argument_node == NULL)
        {
            parser_free_ast((ASTNode *)node);
            return NULL;
        }

        if (node->argument_count == node->argument_capacity)
        {
            node->argument_capacity *= 2;
            node->arguments = realloc(node->arguments,
                                      sizeof(ASTNode *) * node->argument_capacity);
        }
        node->arguments[node->argument_count++] = argument_node;

        parser_advance_token(parser, lexer);
        if (parser->current_token->type == TOKEN_RIGHT_ANGLE)
            break;

        if (parser->current_token->type == TOKEN_SEPARATOR)
        {
            parser_advance_token(parser, lexer);
            if (parser->current_token->type == TOKEN_RIGHT_ANGLE)
            {
                parser_report_unexpected_token(parser, lexer, "E0201",
                                              "Expected an argument after ','");
                parser_free_ast((ASTNode *)node);
                return NULL;
            }

            continue;
        }

        parser_report_unexpected_token(parser, lexer, "E0201",
                                      "Expected ',' or '>' after function argument");
        parser_free_ast((ASTNode *)node);
        return NULL;
    }

    if (parser->current_token->type == TOKEN_END)
    {
        parser_report_unexpected_token(parser, lexer, "E0201",
                                      "Expected '>' after function arguments");
        parser_free_ast((ASTNode *)node);
        return NULL;
    }

    return (ASTNode *)node;
}

static ASTNode *parse_function(Parser *parser, Lexer *lexer);
static int is_statement_start(TokenType type);
static void parser_synchronize(Parser *parser, Lexer *lexer, int inside_block);

static ASTNode *parse_statement(Parser *parser, Lexer *lexer)
{
    switch (parser->current_token->type)
    {
    case TOKEN_RESERVED_WORD_PUSH:
        return parse_push(parser, lexer);
    case TOKEN_RESERVED_WORD_POP:
        return parse_pop(parser);
    case TOKEN_RESERVED_WORD_PRINT:
        return parser_print(parser);
    case TOKEN_RESERVED_WORD_ADD:
        return parser_add(parser);
    case TOKEN_RESERVED_WORD_SUB:
        return parser_sub(parser);
    case TOKEN_RESERVED_WORD_MUL:
        return parser_mul(parser);
    case TOKEN_RESERVED_WORD_DIV:
        return parser_div(parser);
    case TOKEN_RESERVED_WORD_MOD:
        return parser_mod(parser);
    case TOKEN_RESERVED_WORD_FUNC:
        return parse_function(parser, lexer);
    case TOKEN_RESERVED_WORD_CALL_FUNC:
        return parse_call_function(parser, lexer);
    default:
        parser_report_unexpected_token(parser, lexer, "E0200",
                                       "Expected a statement");
        return NULL;
    }
}

static ASTNode *parse_function(Parser *parser, Lexer *lexer)
{
    FunctionNode *node = malloc(sizeof(FunctionNode));
    node->base.type = NODE_FUNCTION;
    node->name = NULL;
    node->params = malloc(sizeof(char *) * 8);
    node->parameter_count = 0;
    node->parameter_capacity = 8;

    BlockNode *block = malloc(sizeof(BlockNode));
    block->base.type = NODE_BLOCK;
    block->statement_capacity = 255;
    block->statement_count = 0;
    block->statements = malloc(sizeof(ASTNode *) * 255);
    node->body = block;

    parser_advance_token(parser, lexer);
    if (parser->current_token->type != TOKEN_IDENTIFIER)
    {
        parser_report_unexpected_token(parser, lexer, "E0202",
                                      "Expected function name after 'func'");
        parser_free_ast((ASTNode *)node);
        return NULL;
    }
    node->name = malloc(strlen(parser->current_token->value) + 1);
    strcpy(node->name, parser->current_token->value);

    parser_advance_token(parser, lexer);
    if (parser->current_token->type != TOKEN_LEFT_ANGLE)
    {
        parser_report_unexpected_token(parser, lexer, "E0202",
                                      "Expected '<' before function parameters");
        parser_free_ast((ASTNode *)node);
        return NULL;
    }

    parser_advance_token(parser, lexer);
    while (parser->current_token->type != TOKEN_RIGHT_ANGLE &&
           parser->current_token->type != TOKEN_END)
    {
        if (parser->current_token->type == TOKEN_SEPARATOR)
        {
            parser_advance_token(parser, lexer);
            continue;
        }

        if (parser->current_token->type != TOKEN_IDENTIFIER)
        {
            parser_report_unexpected_token(parser, lexer, "E0202",
                                          "Expected a parameter name");
            parser_free_ast((ASTNode *)node);
            return NULL;
        }

        if (node->parameter_count == node->parameter_capacity)
        {
            node->parameter_capacity *= 2;
            node->params = realloc(node->params,
                                   sizeof(char *) * node->parameter_capacity);
        }
        size_t length = strlen(parser->current_token->value);
        char *name = malloc(length + 1);
        strcpy(name, parser->current_token->value);
        node->params[node->parameter_count++] = name;

        parser_advance_token(parser, lexer);
    }

    if (parser->current_token->type == TOKEN_END)
    {
        parser_report_unexpected_token(parser, lexer, "E0202",
                                      "Expected '>' after function parameters");
        parser_free_ast((ASTNode *)node);
        return NULL;
    }

    parser_advance_token(parser, lexer);

    if (parser->current_token->type != TOKEN_LEFT_BRACE)
    {
        parser_report_unexpected_token(parser, lexer, "E0202",
                                      "Expected '{' before function body");
        parser_free_ast((ASTNode *)node);
        return NULL;
    }
    parser_advance_token(parser, lexer);

    while (parser->current_token->type != TOKEN_RIGHT_BRACE)
    {
        if (parser->current_token->type == TOKEN_END)
        {
            parser_report_unexpected_token(parser, lexer, "E0202",
                                          "Expected '}' before end of file");
            parser_free_ast((ASTNode *)node);
            return NULL;
        }

        size_t position_before_statement = parser->position;
        ASTNode *statement = parse_statement(parser, lexer);
        if (statement != NULL)
        {
            if (node->body->statement_count == node->body->statement_capacity)
            {
                node->body->statement_capacity *= 2;
                node->body->statements = realloc(
                    node->body->statements,
                    sizeof(ASTNode *) * node->body->statement_capacity);
            }
            node->body->statements[node->body->statement_count++] = statement;
            parser_advance_token(parser, lexer);
        }
        else
        {
            if (parser->position == position_before_statement)
                parser_advance_token(parser, lexer);
            parser_synchronize(parser, lexer, 1);
        }
    }

    return (ASTNode *)node;
}

static int is_statement_start(TokenType type)
{
    switch (type)
    {
    case TOKEN_RESERVED_WORD_PUSH:
    case TOKEN_RESERVED_WORD_POP:
    case TOKEN_RESERVED_WORD_PRINT:
    case TOKEN_RESERVED_WORD_ADD:
    case TOKEN_RESERVED_WORD_SUB:
    case TOKEN_RESERVED_WORD_MUL:
    case TOKEN_RESERVED_WORD_DIV:
    case TOKEN_RESERVED_WORD_MOD:
    case TOKEN_RESERVED_WORD_FUNC:
    case TOKEN_RESERVED_WORD_CALL_FUNC:
        return 1;
    default:
        return 0;
    }
}

static void parser_synchronize(Parser *parser, Lexer *lexer, int inside_block)
{
    while (parser->current_token->type != TOKEN_END)
    {
        if (is_statement_start(parser->current_token->type) ||
            (inside_block && parser->current_token->type == TOKEN_RIGHT_BRACE))
        {
            return;
        }

        parser_advance_token(parser, lexer);
    }
}

void parser_init(Parser *parser, DiagnosticList *diags)
{
    parser->position = 0;
    parser->current_token = NULL;
    parser->diags = diags;
}

ASTNode *parser_code(Parser *parser, Lexer *lexer)
{
    parser_advance_token(parser, lexer);

    while (parser->current_token->type != TOKEN_END)
    {
        if (!is_statement_start(parser->current_token->type))
        {
            if (parser->current_token->type == TOKEN_RIGHT_BRACE)
            {
                parser_report_unexpected_token(parser, lexer, "E0203",
                                              "Unexpected '}' outside a function");
            }
            else
            {
                parser_report_unexpected_token(parser, lexer, "E0200",
                                              "Expected a statement");
            }

            parser_synchronize(parser, lexer, 0);
            continue;
        }

        size_t position_before_statement = parser->position;
        ASTNode *node = parse_statement(parser, lexer);
        if (node != NULL)
            return node;

        if (parser->position == position_before_statement)
            parser_advance_token(parser, lexer);

        parser_synchronize(parser, lexer, 0);
    }

    EndNode *node = malloc(sizeof(EndNode));
    node->base.type = NODE_END;
    return (ASTNode *)node;
}

void parser_free_ast(ASTNode *node)
{
    if (node == NULL)
        return;

    switch (node->type)
    {
    case NODE_INDENTIFIER:
    {
        IdentifierNode *identifier = (IdentifierNode *)node;
        free(identifier->name);
        break;
    }
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
    case NODE_CALL_FUNCTION:
    {
        CallFunctionNode *call = (CallFunctionNode *)node;
        free(call->name);
        for (size_t i = 0; i < call->argument_count; i++)
            parser_free_ast(call->arguments[i]);
        free(call->arguments);
        break;
    }
    case NODE_FUNCTION:
    {
        FunctionNode *function = (FunctionNode *)node;
        free(function->name);
        for (size_t i = 0; i < function->parameter_count; i++)
            free(function->params[i]);
        free(function->params);
        parser_free_ast((ASTNode *)function->body);
        break;
    }
    case NODE_BLOCK:
    {
        BlockNode *block = (BlockNode *)node;
        for (size_t i = 0; i < block->statement_count; i++)
            parser_free_ast(block->statements[i]);
        free(block->statements);
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
