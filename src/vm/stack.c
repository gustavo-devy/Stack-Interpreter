#include "vm/stack.h"

Stack *stack_create(int32_t capacity)
{
    Stack *s = malloc(sizeof(Stack));
    s->capacity = capacity;
    s->top = 0;
    s->data = malloc(capacity * sizeof(uint32_t));

    return s;
}

void stack_destroy(Stack *s)
{
    free(s->data);
    free(s);
}