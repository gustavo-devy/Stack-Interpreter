#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "error/diagnostic.h"

static void render_line_context(const Diagnostic *d, FILE *out)
{
    const char *src = d->location.source;
    size_t off = d->location.offset;

    size_t line_start = off;
    while (line_start > 0 && src[line_start - 1] != '\n')
        line_start--;

    size_t line_end = off;
    while (src[line_end] && src[line_end] != '\n')
        line_end++;

    size_t line = d->location.line;
    size_t column = d->location.column;

    fprintf(out, "  --> %s:%zu:%zu\n", d->location.filename, line, column);

    int width = snprintf(NULL, 0, "%zu", line);
    fprintf(out, "%*zu | ", width, line);
    fwrite(src + line_start, 1, line_end - line_start, out);
    fputc('\n', out);

    fprintf(out, "%*s | ", width, "");
    for (size_t i = 1; i < column; i++)
        fputc(' ', out);
    size_t len = d->location.length ? d->location.length : 1;
    for (size_t i = 0; i < len; i++)
        fputc('^', out);
    if (d->note[0])
        fprintf(out, " %s", d->note);
    fputc('\n', out);
}

void diag_emit(DiagnosticList *list, Severity sev, const char *code,
               SourceLocation location, const char *fmt, ...)
{
    if (list->count >= DIAG_MAX)
        return;

    Diagnostic *d = &list->items[list->count++];
    d->severity = sev;
    d->code = code;
    d->location = location;
    d->note[0] = '\0';

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(d->message, sizeof(d->message), fmt, ap);
    va_end(ap);
}

void diag_render(const Diagnostic *d, FILE *out)
{
    const char *sev = d->severity == SEV_ERROR ? "error" : "warning";
    fprintf(out, "%s[%s]: %s\n", sev, d->code, d->message);
    render_line_context(d, out);
}
