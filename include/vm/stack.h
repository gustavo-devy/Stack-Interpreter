#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <stdint.h>

#define DEFAULT_STACK_SIZE 256

typedef struct Stack
{
    int32_t *data;
    int32_t top;
    int32_t capacity;
} Stack;

Stack *stack_create(int32_t capacity);
void stack_destroy(Stack *s);

#endif // STACK_H
