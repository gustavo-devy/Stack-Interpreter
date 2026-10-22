#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum TokenType
{
    TOKEN_RESERVED_WORD,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_ANGLE,
    TOKEN_RIGHT_ANGLE,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_SEPARATOR,
    TOKEN_END
} TokenType;

typedef enum ReservedWord
{
    RESERVED_WORD_NONE,
    RESERVED_WORD_PUSH,
    RESERVED_WORD_POP,
    RESERVED_WORD_ADD,
    RESERVED_WORD_SUB,
    RESERVED_WORD_MUL,
    RESERVED_WORD_DIV,
    RESERVED_WORD_MOD,
    RESERVED_WORD_FUNC,
    RESERVED_WORD_CALL_FUNC,
    RESERVED_WORD_PRINT,
} ReservedWord;

typedef struct Token
{
    TokenType type;
    const char *value;
    ReservedWord reserved_word;
    int8_t is_reserved_word;
} Token;

typedef struct Lexer
{
    const char *source;
    char current_char;
    size_t position;
} Lexer;

void lexer_init(Lexer *lexer, const char *source);
Token *lexer_next_token(Lexer *lexer);
void lexer_free_token(Token *token);

#endif // LEXER_H
