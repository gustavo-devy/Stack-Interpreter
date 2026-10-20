#ifndef VM_H
#define VM_H

#include <stdint.h>

#include "compile/compile.h"
#include "vm/stack.h"

typedef struct VM
{
    Chunk *chunk;
    u_char *ip;

    Stack *global_stack;
    BlockFrame *current_frame;
} VM;

void vm_init(VM *vm, Chunk *chunk);
void run_vm(VM *vm);

#endif // VM_H
