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

static Token *lexer_create_token(TokenType type, const char *value, ReservedWord reserved_word, int8_t is_reserved_word)
{
    Token *token = (Token *)malloc(sizeof(Token));
    token->type = type;
    token->value = value;
    token->reserved_word = reserved_word;
    token->is_reserved_word = is_reserved_word;
    return token;
}

static Token *lexer_get_identifier(Lexer *lexer)
{
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

    ReservedWord reserved_word = RESERVED_WORD_NONE;
    int8_t is_reserved_word = 0;

    if (strcmp(identifier, "push") == 0)
    {
        reserved_word = RESERVED_WORD_PUSH;
        is_reserved_word = 1;

        return lexer_create_token(TOKEN_RESERVED_WORD, identifier, reserved_word, is_reserved_word);
    }
    else if (strcmp(identifier, "pop") == 0)
    {
        reserved_word = RESERVED_WORD_POP;
        is_reserved_word = 1;

        return lexer_create_token(TOKEN_RESERVED_WORD, identifier, reserved_word, is_reserved_word);
    }
    else if (strcmp(identifier, "add") == 0)
    {
        reserved_word = RESERVED_WORD_ADD;
        is_reserved_word = 1;

        return lexer_create_token(TOKEN_RESERVED_WORD, identifier, reserved_word, is_reserved_word);
    }
    else if (strcmp(identifier, "sub") == 0)
    {
        reserved_word = RESERVED_WORD_SUB;
        is_reserved_word = 1;

        return lexer_create_token(TOKEN_RESERVED_WORD, identifier, reserved_word, is_reserved_word);
    }
    else if (strcmp(identifier, "mul") == 0)
    {
        reserved_word = RESERVED_WORD_MUL;
        is_reserved_word = 1;

        return lexer_create_token(TOKEN_RESERVED_WORD, identifier, reserved_word, is_reserved_word);
    }
    else if (strcmp(identifier, "div") == 0)
    {
        reserved_word = RESERVED_WORD_DIV;
        is_reserved_word = 1;

        return lexer_create_token(TOKEN_RESERVED_WORD, identifier, reserved_word, is_reserved_word);
    }
    else if (strcmp(identifier, "mod") == 0)
    {
        reserved_word = RESERVED_WORD_MOD;
        is_reserved_word = 1;

        return lexer_create_token(TOKEN_RESERVED_WORD, identifier, reserved_word, is_reserved_word);
    }
    else if (strcmp(identifier, "print") == 0)
    {
        reserved_word = RESERVED_WORD_PRINT;
        is_reserved_word = 1;

        return lexer_create_token(TOKEN_RESERVED_WORD, identifier, reserved_word, is_reserved_word);
    }
    else if (strcmp(identifier, "func") == 0)
    {
        reserved_word = RESERVED_WORD_FUNC;
        is_reserved_word = 1;

        return lexer_create_token(TOKEN_RESERVED_WORD, identifier, reserved_word, is_reserved_word);
    }
    else if (strcmp(identifier, "call") == 0)
    {
        reserved_word = RESERVED_WORD_CALL_FUNC;
        is_reserved_word = 1;

        return lexer_create_token(TOKEN_RESERVED_WORD, identifier, reserved_word, is_reserved_word);
    }

    return lexer_create_token(TOKEN_IDENTIFIER, identifier, reserved_word, is_reserved_word);
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

    return lexer_create_token(TOKEN_NUMBER, number, RESERVED_WORD_NONE, 0);
}

static Token *lexer_create_char_token(Lexer *lexer, TokenType type, char ch)
{
    ADD_COLUMN(lexer)

    char *value = malloc(2);
    value[0] = ch;
    value[1] = '\0';
    return lexer_create_token(type, value, RESERVED_WORD_NONE, 0);
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
            printf("Unknown character: %c FILE: %s, COLUMN: %d\n", lexer->current_char, lexer->location.filename, lexer->location.column);
            exit(EXIT_FAILURE);
        }
    }

    char *value = malloc(sizeof(char) * 4);
    strcpy(value, "END");

    return lexer_create_token(TOKEN_END, value, RESERVED_WORD_NONE, 0);
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
