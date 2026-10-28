#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "error/error.h"
#include "error/diagnostic.h"

typedef enum TokenType
{
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
    TOKEN_RESERVED_WORD_PUSH,
    TOKEN_RESERVED_WORD_POP,
    TOKEN_RESERVED_WORD_ADD,
    TOKEN_RESERVED_WORD_SUB,
    TOKEN_RESERVED_WORD_MUL,
    TOKEN_RESERVED_WORD_DIV,
    TOKEN_RESERVED_WORD_MOD,
    TOKEN_RESERVED_WORD_FUNC,
    TOKEN_RESERVED_WORD_CALL_FUNC,
    TOKEN_RESERVED_WORD_PRINT,
    TOKEN_END
} TokenType;

typedef struct Token
{
    TokenType type;
    const char *value;
    TokenLocation location;
} Token;

typedef struct Lexer
{
    const char *source;
    char current_char;
    size_t position;
    DiagnosticList *diags;
    SourceLocation location;
} Lexer;

void lexer_init(Lexer *lexer, const char *filename, const char *source, DiagnosticList *diags);
Token *lexer_next_token(Lexer *lexer);
void lexer_free_token(Token *token);

#endif // LEXER_H
