#ifndef VM_H
#define VM_H

#include <stdint.h>

#include "compile/compile.h"

#define STACK_MAX 256
#define FRAMES_MAX 64

typedef struct CallFrame
{
    Chunk *chunk;
    unsigned char *ip;
    uint32_t stack_base;
    size_t parameter_count;
} CallFrame;

typedef struct VM
{
    Compiler *compiler;
    Chunk *chunk;
    unsigned char *ip;

    int32_t stack[STACK_MAX];
    uint32_t sp;

    CallFrame frames[FRAMES_MAX];
    uint32_t frame_count;
} VM;

void vm_init(VM *vm, Chunk *chunk, Compiler *compiler);
void run_vm(VM *vm);

#endif // VM_H
