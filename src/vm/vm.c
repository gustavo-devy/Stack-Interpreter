#include <stdio.h>

#include "vm/vm.h"

#define READ_BYTE() (*vm->ip++)
#define READ_CONSTANT() (vm->chunk->constants[READ_BYTE()])

void vm_init(VM *vm, Chunk *chunk)
{
    vm->chunk = chunk;
    vm->ip = chunk->code;
    vm->stack_top = vm->stack;
}

void run_vm(VM *vm)
{
    for (;;)
    {
        unsigned char instruction = READ_BYTE();

        switch (instruction)
        {
        case OP_CONSTANT:
        {
            int value = READ_CONSTANT();
            if (vm->stack_top - vm->stack >= STACK_MAX)
            {
                fprintf(stderr, "Stack overflow.\n");
                return;
            }

            *vm->stack_top++ = value;
            break;
        }

        case OP_PUSH_CMD:
        {
            if (vm->stack_top == vm->stack)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            break;
        }

        case OP_POP_CMD:
        {
            if (vm->stack_top == vm->stack)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->stack_top--;
            break;
        }

        case OP_PRINT_CMD:
        {
            printf("%d\n",
                   *(vm->stack_top - 1));
            break;
        }

        case OP_ADD:
        {
            if (vm->stack_top - vm->stack < 2)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->stack_top--;
            int right = *vm->stack_top;
            vm->stack_top--;
            int left = *vm->stack_top;
            *vm->stack_top++ = left + right;
            break;
        }

        case OP_SUBTRACT:
        {
            if (vm->stack_top - vm->stack < 2)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->stack_top--;
            int right = *vm->stack_top;
            vm->stack_top--;
            int left = *vm->stack_top;
            *vm->stack_top++ = left - right;
            break;
        }

        case OP_MULTIPLICATION:
        {
            if (vm->stack_top - vm->stack < 2)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->stack_top--;
            int right = *vm->stack_top;
            vm->stack_top--;
            int left = *vm->stack_top;
            *vm->stack_top++ = left * right;
            break;
        }

        case OP_DIVISION:
        {
            if (vm->stack_top - vm->stack < 2)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->stack_top--;
            int right = *vm->stack_top;
            vm->stack_top--;
            int left = *vm->stack_top;
            *vm->stack_top++ = left / right;
            break;
        }

        case OP_HALT:
        {
            return;
        }

        default:
        {
            fprintf(stderr, "Instrução desconhecida: %d\n", instruction);
            return;
        }
        }
    }
}
