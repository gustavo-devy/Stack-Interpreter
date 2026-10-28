#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

#include <stdio.h>

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
    SourceLocation location;
} Diagnostic;

typedef struct DiagnosticList
{
    Diagnostic items[DIAG_MAX];
    int count;
} DiagnosticList;

void diag_emit(DiagnosticList *list, Severity sev, const char *code,
               SourceLocation location, const char *fmt, ...);
void diag_render(const Diagnostic *d, FILE *out);

#endif // DIAGNOSTIC_H
