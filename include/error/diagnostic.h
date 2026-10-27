#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

#include "error.h"

#define DIAG_MAX 64

typedef enum Severity 
{
    SEV_ERROR,
    SEV_WARNING,
} Severity;

typedef struct Diagnostic
{
    Severity severity;
    const char *code;
    char message[256];
    char note[256];
    SourceLocation loc;
} Diagnostic;

typedef struct DiagnosticList 
{
    Diagnostic items[DIAG_MAX];
    int count;
} DiagnosticList;

#endif // DIAGNOSTIC_H

