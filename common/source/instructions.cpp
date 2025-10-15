#include "instructions.h"

#include <math.h>

#include "processor.h"

ProcessorError PUSH(const int* args, Processor* processor) {
    ProcessorCheck(processor);
    
    int elem = args[0];

    PROCESSOR_STACK_DO_OR_DIE(StackPush(&processor->stack, elem), processor);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError POP(const int* /*args*/, Processor* processor) {
    ProcessorCheck(processor);

    int poped_elem = 0;
    StackError error_code = StackPop(&processor->stack, &poped_elem);

    if (error_code != STACK_OK) {
        return processor->last_error_code = STACK_ERROR;
    }

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError ADD(const int* /*args*/, Processor* processor) {
    ProcessorCheck(processor);

    int a = 0;
    int b = 0;

    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &a), processor);
    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &b), processor);

    PROCESSOR_STACK_DO_OR_DIE(StackPush(&processor->stack, a + b), processor);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError SUB(const int* /*args*/, Processor* processor) {
    ProcessorCheck(processor);

    int a = 0;
    int b = 0;

    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &a), processor);
    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &b), processor);

    PROCESSOR_STACK_DO_OR_DIE(StackPush(&processor->stack, a - b), processor);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError MUL(const int* /*args*/, Processor* processor) {
    ProcessorCheck(processor);

    int a = 0;
    int b = 0;

    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &a), processor);
    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &b), processor);

    PROCESSOR_STACK_DO_OR_DIE(StackPush(&processor->stack, a * b), processor);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError DIV(const int* /*args*/, Processor* processor) {
    ProcessorCheck(processor);

    int a = 0;
    int b = 0;

    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &a), processor);
    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &b), processor);

    PROCESSOR_STACK_DO_OR_DIE(StackPush(&processor->stack, a / b), processor);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError SQRT(const int* /*args*/, Processor* processor) {
    ProcessorCheck(processor);

    int a = 0;

    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &a), processor);

    PROCESSOR_STACK_DO_OR_DIE(StackPush(&processor->stack, (int)floor(sqrt(a))), processor);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError OUT(const int* /*args*/, Processor* processor) {
    ProcessorCheck(processor);

    int a = 0;

    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &a), processor);

    printf("%d\n", a);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError IN(const int* /*args*/, Processor* processor) {
    ProcessorCheck(processor);

    int a = 0;

    scanf("%d", &a);

    PROCESSOR_STACK_DO_OR_DIE(StackPush(&processor->stack, a), processor);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError PUSHR(const int* args, Processor* processor) {
    ProcessorCheck(processor);

    int reg_i = args[0];

    PROCESSOR_STACK_DO_OR_DIE(StackPush(&processor->stack, processor->regs[reg_i]), processor);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError POPR(const int* args, Processor* processor) {
    ProcessorCheck(processor);

    int reg_i = args[0];

    PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &processor->regs[reg_i]), processor);

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError JMP(const int* args, Processor* processor) {
    ProcessorCheck(processor);

    size_t adress = (size_t)args[0];

    processor->instruction_ptr = adress - 1;

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

#define DefJ(name, operator) \
    ProcessorError J##name(const int* args, Processor* processor) {            \
        ProcessorCheck(processor);                                             \
                                                                               \
        int a = 0;                                                             \
        int b = 0;                                                             \
                                                                               \
        PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &a), processor); \
        PROCESSOR_STACK_DO_OR_DIE(StackPop(&processor->stack, &b), processor); \
                                                                               \
        if (a operator b) {                                                    \
            size_t adress = (size_t)args[0];                                   \
                                                                               \
            processor->instruction_ptr = adress - 1;                           \
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

ProcessorError COM(const int* /*args*/, Processor* processor) {
    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError CALL(const int* args, Processor* processor) {
    ProcessorCheck(processor);

    StackPush(&processor->call_stack, (int)processor->instruction_ptr + 1);

    size_t adress = (size_t)args[0];

    processor->instruction_ptr = adress - 1;

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError RET(const int* /*args*/, Processor* processor) {
    ProcessorCheck(processor);

    int adress = 0;

    StackPop(&processor->call_stack, &adress);

    processor->instruction_ptr = (size_t)adress - 1;

    ProcessorCheck(processor);

    return processor->last_error_code = PROCESSOR_OK;
}