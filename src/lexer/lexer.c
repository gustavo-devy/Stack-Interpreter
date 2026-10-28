#include "lexer/lexer.h"

// static SourceLocation loc_at(Lexer *lexer, size_t offset, size_t length)
// {
// }

static void lexer_advance(Lexer *lexer)
{
    if (lexer->current_char == '\n')
    {
        lexer->location.line++;
        lexer->location.column = 1;
    }
    else if (lexer->current_char != '\0')
    {
        lexer->location.column++;
    }

    lexer->position++;
    lexer->current_char = lexer->source[lexer->position];
    lexer->location.offset = lexer->position;
    lexer->location.length = lexer->current_char == '\0' ? 0 : 1;
}

static void lexer_skip_whitespace(Lexer *lexer)
{
    while (isspace(lexer->current_char))
    {
        lexer_advance(lexer);
    }
}

static Token *lexer_create_token(TokenType type, const char *value, TokenLocation location)
{
    Token *token = (Token *)malloc(sizeof(Token));
    token->type = type;
    token->value = value;
    token->location = location;

    return token;
}

static Token *lexer_get_identifier(Lexer *lexer)
{
    static const struct
    {
        const char *name;
        TokenType type;
    } reserved_words[] = {
        {"push", TOKEN_RESERVED_WORD_PUSH},
        {"pop", TOKEN_RESERVED_WORD_POP},
        {"add", TOKEN_RESERVED_WORD_ADD},
        {"sub", TOKEN_RESERVED_WORD_SUB},
        {"mul", TOKEN_RESERVED_WORD_MUL},
        {"div", TOKEN_RESERVED_WORD_DIV},
        {"mod", TOKEN_RESERVED_WORD_MOD},
        {"func", TOKEN_RESERVED_WORD_FUNC},
        {"call", TOKEN_RESERVED_WORD_CALL_FUNC},
        {"print", TOKEN_RESERVED_WORD_PRINT},
    };

    TokenLocation location;

    size_t start_position = lexer->position;

    location.start = start_position;
    location.filename = lexer->location.filename;
    location.column = lexer->location.column;
    location.line = lexer->location.line;

    while (isalpha(lexer->current_char) || isdigit(lexer->current_char) || lexer->current_char == '_')
    {
        lexer_advance(lexer);
    }

    size_t length = lexer->position - start_position;
    char *identifier = (char *)malloc(length + 1);
    strncpy(identifier, lexer->source + start_position, length);
    identifier[length] = '\0';

    location.end = lexer->position;

    for (size_t i = 0; i < sizeof(reserved_words) / sizeof(reserved_words[0]); i++)
    {
        if (strcmp(identifier, reserved_words[i].name) == 0)
        {
            return lexer_create_token(reserved_words[i].type, identifier, location);
        }
    }

    return lexer_create_token(TOKEN_IDENTIFIER, identifier, location);
}

static Token *lexer_get_number(Lexer *lexer)
{
    TokenLocation location;

    size_t start_position = lexer->position;

    location.start = start_position;
    location.filename = lexer->location.filename;
    location.column = lexer->location.column;
    location.line = lexer->location.line;

    while (isdigit(lexer->current_char))
    {
        lexer_advance(lexer);
    }

    size_t length = lexer->position - start_position;
    char *number = (char *)malloc(length + 1);
    strncpy(number, lexer->source + start_position, length);
    number[length] = '\0';

    location.end = lexer->position;

    return lexer_create_token(TOKEN_NUMBER, number, location);
}

static Token *lexer_create_char_token(Lexer *lexer, TokenType type, char ch)
{
    TokenLocation location;

    location.start = lexer->position;
    location.end = location.start + 1;
    location.filename = lexer->location.filename;
    location.column = lexer->location.column;
    location.line = lexer->location.line;

    char *value = malloc(2);
    value[0] = ch;
    value[1] = '\0';

    Token *token = lexer_create_token(type, value, location);
    lexer_advance(lexer);
    return token;
}

void lexer_init(Lexer *lexer, const char *filename, const char *source, DiagnosticList *diags)
{
    lexer->source = source;
    lexer->position = 0;
    lexer->current_char = source[0];
    lexer->location.filename = filename;
    lexer->location.source = source;
    lexer->location.line = 1;
    lexer->location.column = 1;
    lexer->location.offset = 0;
    lexer->location.length = source[0] == '\0' ? 0 : 1;
    lexer->diags = diags;
}

Token *lexer_next_token(Lexer *lexer)
{
    while (lexer->current_char != '\0')
    {
        if (isspace(lexer->current_char))
        {
            lexer_skip_whitespace(lexer);
            continue;
        }

        if (isalpha(lexer->current_char) || lexer->current_char == '_')
        {
            return lexer_get_identifier(lexer);
        }

        if (isdigit(lexer->current_char))
        {
            return lexer_get_number(lexer);
        }

        switch (lexer->current_char)
        {
        case '+':
            return lexer_create_char_token(lexer, TOKEN_PLUS, '+');
        case '-':
            return lexer_create_char_token(lexer, TOKEN_MINUS, '-');
        case '*':
            return lexer_create_char_token(lexer, TOKEN_STAR, '*');
        case '/':
            return lexer_create_char_token(lexer, TOKEN_SLASH, '/');
        case '(':
            return lexer_create_char_token(lexer, TOKEN_LEFT_PAREN, '(');
        case ')':
            return lexer_create_char_token(lexer, TOKEN_RIGHT_PAREN, ')');
        case '<':
            return lexer_create_char_token(lexer, TOKEN_LEFT_ANGLE, '<');
        case '>':
            return lexer_create_char_token(lexer, TOKEN_RIGHT_ANGLE, '>');
        case '{':
            return lexer_create_char_token(lexer, TOKEN_LEFT_BRACE, '{');
        case '}':
            return lexer_create_char_token(lexer, TOKEN_RIGHT_BRACE, '}');
        case ',':
            return lexer_create_char_token(lexer, TOKEN_SEPARATOR, ',');
        default:
            diag_emit(lexer->diags, SEV_ERROR, "E0002", lexer->location,
                      "invalid character '%c'", lexer->current_char);
            diag_render(&lexer->diags->items[lexer->diags->count - 1], stderr);
            exit(EXIT_FAILURE);
        }
    }

    char *value = malloc(sizeof(char) * 4);
    strcpy(value, "END");

    return lexer_create_token(TOKEN_END, value, (TokenLocation){.filename = lexer->location.filename, .start = lexer->position, .column = lexer->location.column, .line = lexer->location.line, .end = lexer->position});
}

void lexer_free_token(Token *token)
{
    if (token == NULL)
        return;

    if (token->value == NULL)
    {
        free(token);
        token = NULL;

        return;
    }

    free((void *)token->value);
    token->value = NULL;

    free(token);
    token = NULL;
}
