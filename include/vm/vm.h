#ifndef VM_H
#define VM_H

#define STACK_MAX 256

#include "compile/compile.h"

typedef struct
{
    Chunk *chunk;
    unsigned char *ip;

    int stack[STACK_MAX];
    int *stack_top;
} VM;

void vm_init(VM *vm, Chunk *chunk);
void run_vm(VM *vm);

#endif // VM_H
