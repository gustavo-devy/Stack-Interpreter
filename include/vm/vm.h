#ifndef VM_H
#define VM_H

#include <stdint.h>

#include "compile/compile.h"

#define STACK_MAX 256
#define FRAMES_MAX 64

typedef struct VM
{
    Chunk *chunk;
    u_char *ip;

    int32_t stack[STACK_MAX];
    uint32_t sp;

    uint32_t frames[FRAMES_MAX];
    uint32_t fp;
} VM;

void vm_init(VM *vm, Chunk *chunk);
void run_vm(VM *vm);

#endif // VM_H
