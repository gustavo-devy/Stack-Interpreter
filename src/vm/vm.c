#include <stdio.h>
#include <inttypes.h>

#include "vm/vm.h"

#define READ_BYTE() (*vm->ip++)
#define READ_CONSTANT() (vm->chunk->constants[READ_BYTE()])

void vm_init(VM *vm, Chunk *chunk)
{
    vm->chunk = chunk;
    vm->ip = chunk->code;
    vm->current_frame = NULL;
    vm->global_stack = stack_create(DEFAULT_STACK_SIZE);
}

void run_vm(VM *vm)
{
    for (;;)
    {
        u_char instruction = READ_BYTE();

        switch (instruction)
        {
        case OP_CONSTANT:
        {
            int32_t value = READ_CONSTANT();
            if (vm->global_stack->top >= vm->global_stack->capacity)
            {
                fprintf(stderr, "Stack overflow.\n");
                return;
            }

            vm->global_stack->data[vm->global_stack->top++] = value;

            break;
        }

        case OP_PUSH_CMD:
        {
            if (vm->global_stack->top == 0)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            break;
        }

        case OP_POP_CMD:
        {
            if (vm->global_stack->top == 0)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->global_stack->top--;

            break;
        }

        case OP_ADD_CMD:
        {
            if (vm->global_stack->top < 2)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->global_stack->top--;
            uint32_t right = vm->global_stack->data[vm->global_stack->top];
            vm->global_stack->top--;
            uint32_t left = vm->global_stack->data[vm->global_stack->top];
            vm->global_stack->data[vm->global_stack->top++] = left + right;

            break;
        }

        case OP_SUB_CMD:
        {
            if (vm->global_stack->top < 2)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->global_stack->top--;
            uint32_t right = vm->global_stack->data[vm->global_stack->top];
            vm->global_stack->top--;
            uint32_t left = vm->global_stack->data[vm->global_stack->top];
            vm->global_stack->data[vm->global_stack->top++] = left - right;

            break;
        }

        case OP_MUL_CMD:
        {
            if (vm->global_stack->top < 2)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->global_stack->top--;
            uint32_t right = vm->global_stack->data[vm->global_stack->top];
            vm->global_stack->top--;
            uint32_t left = vm->global_stack->data[vm->global_stack->top];
            vm->global_stack->data[vm->global_stack->top++] = left * right;

            break;
        }

        case OP_DIV_CMD:
        {
            if (vm->global_stack->top < 2)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->global_stack->top--;
            uint32_t right = vm->global_stack->data[vm->global_stack->top];
            vm->global_stack->top--;
            uint32_t left = vm->global_stack->data[vm->global_stack->top];
            vm->global_stack->data[vm->global_stack->top++] = left / right;

            break;
        }

        case OP_MOD_CMD:
        {
            if (vm->global_stack->top < 2)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->global_stack->top--;
            uint32_t right = vm->global_stack->data[vm->global_stack->top];
            vm->global_stack->top--;
            uint32_t left = vm->global_stack->data[vm->global_stack->top];
            vm->global_stack->data[vm->global_stack->top++] = left % right;

            break;
        }

        case OP_PRINT_CMD:
        {
            if (vm->global_stack->top == 0)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            printf("%" PRId32 "\n", vm->global_stack->data[vm->global_stack->top - 1]);
            break;
        }

        case OP_ADD:
        {
            if (vm->global_stack->top < 2)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->global_stack->top--;
            uint32_t right = vm->global_stack->data[vm->global_stack->top];
            vm->global_stack->top--;
            uint32_t left = vm->global_stack->data[vm->global_stack->top];
            vm->global_stack->data[vm->global_stack->top++] = left + right;

            break;
        }

        case OP_SUBTRACT:
        {
            if (vm->global_stack->top < 2)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->global_stack->top--;
            uint32_t right = vm->global_stack->data[vm->global_stack->top];
            vm->global_stack->top--;
            uint32_t left = vm->global_stack->data[vm->global_stack->top];
            vm->global_stack->data[vm->global_stack->top++] = left - right;

            break;
        }

        case OP_MULTIPLICATION:
        {
            if (vm->global_stack->top < 2)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->global_stack->top--;
            uint32_t right = vm->global_stack->data[vm->global_stack->top];
            vm->global_stack->top--;
            uint32_t left = vm->global_stack->data[vm->global_stack->top];
            vm->global_stack->data[vm->global_stack->top++] = left * right;

            break;
        }

        case OP_DIVISION:
        {
            if (vm->global_stack->top < 2)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->global_stack->top--;
            uint32_t right = vm->global_stack->data[vm->global_stack->top];
            vm->global_stack->top--;
            uint32_t left = vm->global_stack->data[vm->global_stack->top];
            vm->global_stack->data[vm->global_stack->top++] = left / right;

            break;
        }

        case OP_HALT:
        {
            return;
        }

        default:
        {
            fprintf(stderr, "Unknown instruction: %d\n", instruction);
            return;
        }
        }
    }
}
