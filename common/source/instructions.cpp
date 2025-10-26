#include "instructions.h"

#include <math.h>
#include <assert.h>

#include "processor.h"

ProcessorError PUSH(Processor* processor, const int* args) {
    ProcessorCheck(processor);
    assert(args != NULL);
    
    int elem = args[0];

    PROCESSOR_STACK_DO_OR_DIE(StackPush(&processor->stack, elem), processor);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError POP(Processor* processor, const int* /*args*/) {
    ProcessorCheck(processor);

    int poped_elem = 0;
    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &poped_elem), processor);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError ADD(Processor* processor, const int* /*args*/) {
    ProcessorCheck(processor);

    int a = 0;
    int b = 0;

    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &a), processor);
    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &b), processor);

    PROCESSOR_STACK_DO_OR_DIE(StackPush(&processor->stack, a + b), processor);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError SUB(Processor* processor, const int* /*args*/) {
    ProcessorCheck(processor);

    int a = 0;
    int b = 0;

    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &a), processor);
    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &b), processor);

    PROCESSOR_STACK_DO_OR_DIE(StackPush(&processor->stack, a - b), processor);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError MUL(Processor* processor, const int* /*args*/) {
    ProcessorCheck(processor);

    int a = 0;
    int b = 0;

    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &a), processor);
    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &b), processor);

    PROCESSOR_STACK_DO_OR_DIE(StackPush(&processor->stack, a * b), processor);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError DIV(Processor* processor, const int* /*args*/) {
    ProcessorCheck(processor);

    int a = 0;
    int b = 0;

    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &a), processor);
    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &b), processor);

    if (b == 0) {
        return processor->last_error_code = PROCESSOR_DIV_BY_ZERO_ERROR;
    }

    PROCESSOR_STACK_DO_OR_DIE(StackPush(&processor->stack, a / b), processor);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError SQRT(Processor* processor, const int* /*args*/) {
    ProcessorCheck(processor);

    int a = 0;

    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &a), processor);

    PROCESSOR_STACK_DO_OR_DIE(StackPush(&processor->stack, (int)floor(sqrt(a))), processor);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError OUT(Processor* processor, const int* /*args*/) {
    ProcessorCheck(processor);

    int a = 0;

    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &a), processor);

    printf("%d\n", a);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError IN(Processor* processor, const int* /*args*/) {
    ProcessorCheck(processor);

    int a = 0;

    scanf("%d", &a);

    PROCESSOR_STACK_DO_OR_DIE(StackPush(&processor->stack, a), processor);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError PUSHR(Processor* processor, const int* args) {
    ProcessorCheck(processor);

    int reg_i = args[0];

    PROCESSOR_STACK_DO_OR_DIE(StackPush(&processor->stack, processor->regs[reg_i]), processor);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError POPR(Processor* processor, const int* args) {
    ProcessorCheck(processor);

    int reg_i = args[0];

    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &processor->regs[reg_i]), processor);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError JMP(Processor* processor, const int* args) {
    ProcessorCheck(processor);

    size_t address = (size_t)args[0];

    processor->instruction_ptr = address - 1;

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

#define DefJ(name, operator) \
    ProcessorError J##name(Processor* processor, const int* args) {            \
        ProcessorCheck(processor);                                             \
                                                                               \
        int a = 0;                                                             \
        int b = 0;                                                             \
                                                                               \
        PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &a), processor); \
        PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &b), processor); \
                                                                               \
        if (a operator b) {                                                    \
            size_t address = (size_t)args[0];                                   \
                                                                               \
            processor->instruction_ptr = address - 1;                           \
                                                                               \
            ProcessorCheck(processor);                                         \
        }                                                                      \
                                                                               \
        return processor->last_error_code = PROCESSOR_OK;                      \
    }

DefJ(E, ==);

DefJ(NE, !=);

DefJ(B, <);

DefJ(BE, <=);

DefJ(A, >);

DefJ(AE, >=);

ProcessorError COM(Processor* processor, const int* /*args*/) { // FIXME бля пиздаа че это
    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError CALL(Processor* processor, const int* args) {
    ProcessorCheck(processor);

    PROCESSOR_STACK_DO_OR_DIE(StackPush(&processor->call_stack, (int)processor->instruction_ptr + 1), processor);

    size_t address = (size_t)args[0];

    processor->instruction_ptr = address - 1;

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError RET(Processor* processor, const int* /*args*/) {
    ProcessorCheck(processor);

    int address = 0;

    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->call_stack, &address), processor);

    processor->instruction_ptr = (size_t)address - 1;

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError PUSHM(Processor* processor, const int* args) {
    ProcessorCheck(processor);

    int mem_i = args[0];

    PROCESSOR_STACK_DO_OR_DIE(StackPush(&processor->stack, processor->mem[mem_i]), processor);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError POPM(Processor* processor, const int* args) {
    ProcessorCheck(processor);

    int mem_i = args[0];

    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &processor->mem[mem_i]), processor);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError PUSHMR(Processor* processor, const int* args) {
    ProcessorCheck(processor);

    int reg_i = args[0];

    int mem_i = processor->regs[reg_i];

    PROCESSOR_STACK_DO_OR_DIE(StackPush(&processor->stack, processor->mem[mem_i]), processor);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError POPMR(Processor* processor, const int* args) {
    ProcessorCheck(processor);

    int reg_i = args[0];

    int mem_i = processor->regs[reg_i];

    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &processor->mem[mem_i]), processor);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}