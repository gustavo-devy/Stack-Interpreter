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
    TOKEN_END
} TokenType;

typedef enum ReservedWord
{
    RESERVED_WORD_NONE,
    RESERVED_WORD_PUSH,
    RESERVED_WORD_POP,
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
