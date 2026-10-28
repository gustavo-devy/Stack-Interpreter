#include "lexer/lexer.h"

#define ADD_COLUMN(lexer) lexer->location.column++;

static void lexer_advance(Lexer *lexer)
{
    lexer->position++;
    lexer->current_char = lexer->source[lexer->position];
}

static void lexer_skip_whitespace(Lexer *lexer)
{
    while (isspace(lexer->current_char))
    {
        lexer_advance(lexer);
        ADD_COLUMN(lexer)
    }
}

static Token *lexer_create_token(TokenType type, const char *value)
{
    Token *token = (Token *)malloc(sizeof(Token));
    token->type = type;
    token->value = value;
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

    size_t start_position = lexer->position;
    while (isalpha(lexer->current_char) || lexer->current_char == '_')
    {
        lexer_advance(lexer);
        ADD_COLUMN(lexer)
    }

    size_t length = lexer->position - start_position;
    char *identifier = (char *)malloc(length + 1);
    strncpy(identifier, lexer->source + start_position, length);
    identifier[length] = '\0';

    for (size_t i = 0; i < sizeof(reserved_words) / sizeof(reserved_words[0]); i++)
    {
        if (strcmp(identifier, reserved_words[i].name) == 0)
        {
            return lexer_create_token(reserved_words[i].type, identifier);
        }
    }

    return lexer_create_token(TOKEN_IDENTIFIER, identifier);
}

static Token *lexer_get_number(Lexer *lexer)
{
    size_t start_position = lexer->position;
    while (isdigit(lexer->current_char))
    {
        lexer_advance(lexer);
        ADD_COLUMN(lexer)
    }

    size_t length = lexer->position - start_position;
    char *number = (char *)malloc(length + 1);
    strncpy(number, lexer->source + start_position, length);
    number[length] = '\0';

    return lexer_create_token(TOKEN_NUMBER, number);
}

static Token *lexer_create_char_token(Lexer *lexer, TokenType type, char ch)
{
    ADD_COLUMN(lexer)

    char *value = malloc(2);
    value[0] = ch;
    value[1] = '\0';
    return lexer_create_token(type, value);
}

void lexer_init(Lexer *lexer, const char *filename, const char *source)
{
    lexer->source = source;
    lexer->position = 0;
    lexer->current_char = source[0];
    lexer->location.filename = filename;
    lexer->location.line = 0;
    lexer->location.column = 1;
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
            lexer_advance(lexer);
            return lexer_create_char_token(lexer, TOKEN_PLUS, '+');
        case '-':
            lexer_advance(lexer);
            return lexer_create_char_token(lexer, TOKEN_MINUS, '-');
        case '*':
            lexer_advance(lexer);
            return lexer_create_char_token(lexer, TOKEN_STAR, '*');
        case '/':
            lexer_advance(lexer);
            return lexer_create_char_token(lexer, TOKEN_SLASH, '/');
        case '(':
            lexer_advance(lexer);
            return lexer_create_char_token(lexer, TOKEN_LEFT_PAREN, '(');
        case ')':
            lexer_advance(lexer);
            return lexer_create_char_token(lexer, TOKEN_RIGHT_PAREN, ')');
        case '<':
            lexer_advance(lexer);
            return lexer_create_char_token(lexer, TOKEN_LEFT_ANGLE, '<');
        case '>':
            lexer_advance(lexer);
            return lexer_create_char_token(lexer, TOKEN_RIGHT_ANGLE, '>');
        case '{':
            lexer_advance(lexer);
            return lexer_create_char_token(lexer, TOKEN_LEFT_BRACE, '{');
        case '}':
            lexer_advance(lexer);
            return lexer_create_char_token(lexer, TOKEN_RIGHT_BRACE, '}');
        case ',':
            lexer_advance(lexer);
            return lexer_create_char_token(lexer, TOKEN_SEPARATOR, ',');
        default:
            printf("Unknown character: %c FILE: %s, COLUMN: %lu\n", lexer->current_char, lexer->location.filename, lexer->location.column);
            exit(EXIT_FAILURE);
        }
    }

    char *value = malloc(sizeof(char) * 4);
    strcpy(value, "END");

    return lexer_create_token(TOKEN_END, value);
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
