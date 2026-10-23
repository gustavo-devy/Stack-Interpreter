#include <stdio.h>

#include "vm/vm.h"

#define READ_BYTE() (*vm->ip++)
#define READ_CONSTANT() (vm->chunk->constants[READ_BYTE()])

void vm_init(VM *vm, Chunk *chunk, Compiler *compiler)
{
    vm->compiler = compiler;
    vm->chunk = chunk;
    vm->ip = chunk->code;
    vm->sp = 0;
    memset(&vm->frames, 0, FRAMES_MAX);
    vm->frame_count = 0;
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
            if (vm->sp >= STACK_MAX)
            {
                fprintf(stderr, "Stack overflow.\n");
                return;
            }

            vm->stack[vm->sp++] = value;

            break;
        }

        case OP_GET_PARAMETER:
        {
            uint8_t index = READ_BYTE();

            if (vm->frame_count == 0)
            {
                fprintf(stderr, "Parâmetro usado fora de uma função.\n");
                exit(0);
                return;
            }

            CallFrame *frame = &vm->frames[vm->frame_count - 1];
            if (index >= frame->parameter_count)
            {
                fprintf(stderr, "Índice de parâmetro inválido.\n");
                exit(0);
                return;
            }

            if (vm->sp >= STACK_MAX)
            {
                fprintf(stderr, "Stack overflow.\n");
                exit(0);
                return;
            }

            vm->stack[vm->sp++] = vm->stack[frame->stack_base + index];
            break;
        }

        case OP_CALL_FUNCTION:
        {
            unsigned char function_index = READ_BYTE();
            FunctionSymbol *function = vm->compiler->functions[function_index];

            uint32_t stack_base = vm->sp - function->parameter_count;

            vm->frames[vm->frame_count++] = (CallFrame){
                .chunk = vm->chunk,
                .ip = vm->ip,
                .stack_base = stack_base,
                .parameter_count = function->parameter_count //
            };

            vm->chunk = function->chunk;
            vm->ip = function->chunk->code;
            break;
        }

        case OP_RETURN:
        {
            if (vm->frame_count == 0)
                return;

            CallFrame frame = vm->frames[--vm->frame_count];
            vm->chunk = frame.chunk;
            vm->ip = frame.ip;
            vm->sp = frame.stack_base;
            break;
        }

        case OP_PUSH_CMD:
        {
            if (vm->sp == 0)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            break;
        }

        case OP_POP_CMD:
        {
            if (vm->sp == 0)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->sp--;

            break;
        }

        case OP_ADD_CMD:
        {
            if (vm->sp < 2)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->sp--;
            uint32_t right = vm->stack[vm->sp];
            vm->sp--;
            uint32_t left = vm->stack[vm->sp];
            vm->stack[vm->sp++] = right + left;

            break;
        }

        case OP_SUB_CMD:
        {
            if (vm->sp == 0)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->sp--;
            uint32_t right = vm->stack[vm->sp];
            vm->sp--;
            uint32_t left = vm->stack[vm->sp];
            vm->stack[vm->sp++] = right - left;

            break;
        }

        case OP_MUL_CMD:
        {
            if (vm->sp < 2)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->sp--;
            uint32_t right = vm->stack[vm->sp];
            vm->sp--;
            uint32_t left = vm->stack[vm->sp];
            vm->stack[vm->sp++] = right * left;

            break;
        }

        case OP_DIV_CMD:
        {
            if (vm->sp < 2)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->sp--;
            uint32_t right = vm->stack[vm->sp];
            vm->sp--;
            uint32_t left = vm->stack[vm->sp];
            vm->stack[vm->sp++] = right / left;

            break;
        }

        case OP_MOD_CMD:
        {
            if (vm->sp < 2)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->sp--;
            uint32_t right = vm->stack[vm->sp];
            vm->sp--;
            uint32_t left = vm->stack[vm->sp];
            vm->stack[vm->sp++] = right % left;

            break;
        }

        case OP_PRINT_CMD:
        {
            if (vm->sp == 0)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            printf("%d\n", vm->stack[vm->sp - 1]);
            break;
        }

        case OP_ADD:
        {
            if (vm->sp < 2)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->sp--;
            uint32_t right = vm->stack[vm->sp];
            vm->sp--;
            uint32_t left = vm->stack[vm->sp];
            vm->stack[vm->sp++] = right + left;

            break;
        }

        case OP_SUBTRACT:
        {
            if (vm->sp < 2)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->sp--;
            uint32_t right = vm->stack[vm->sp];
            vm->sp--;
            uint32_t left = vm->stack[vm->sp];
            vm->stack[vm->sp++] = right - left;

            break;
        }

        case OP_MULTIPLICATION:
        {
            if (vm->sp < 2)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->sp--;
            uint32_t right = vm->stack[vm->sp];
            vm->sp--;
            uint32_t left = vm->stack[vm->sp];
            vm->stack[vm->sp++] = right * left;

            break;
        }

        case OP_DIVISION:
        {
            if (vm->sp < 2)
            {
                fprintf(stderr, "Stack underflow.\n");
                return;
            }

            vm->sp--;
            uint32_t right = vm->stack[vm->sp];
            vm->sp--;
            uint32_t left = vm->stack[vm->sp];
            vm->stack[vm->sp++] = right / left;

            break;
        }

        case OP_HALT:
        {
            return;
        }

        default:
        {
            fprintf(stderr, "Unknown instruction: %d\n", instruction);
            exit(0);
        }
        }
    }
}
