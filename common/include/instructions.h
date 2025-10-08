#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_

#include <stdio.h>

#include "processor.h"

typedef ProcessorError (*InstructionFunc) (const int* args, Processor* processor);

#define PROCESSOR_STACK_DO_OR_DIE(func, processor)              \
    func;                                                        \
    if (processor->stack.last_error_code != STACK_OK) {         \
        return STACK_ERROR;                                     \
    }

/// @brief Добавляет elem в конец стэка
/// @param args { elem }
/// @param processor Процессор в котором выполняется инструкция
/// @return Код ошибки
ProcessorError PUSH(const int* args, Processor* processor);

ProcessorError POP(const int* /*args*/, Processor* processor);

ProcessorError ADD(const int* /*args*/, Processor* processor);

ProcessorError SUB(const int* /*args*/, Processor* processor);

ProcessorError MUL(const int* /*args*/, Processor* processor);

ProcessorError DIV(const int* /*args*/, Processor* processor);

ProcessorError SQRT(const int* /*args*/, Processor* processor);

ProcessorError OUT(const int* /*args*/, Processor* processor);

ProcessorError IN(const int* /*args*/, Processor* processor);

ProcessorError PUSHR(const int* /*args*/, Processor* processor);

ProcessorError POPR(const int* /*args*/, Processor* processor);

ProcessorError JMP(const int* /*args*/, Processor* processor);

struct Instruction {
    const char* name;
    const size_t args_count;
    InstructionFunc func;
};

static const Instruction instructions[] = {
    { .name = "PUSH", .args_count = 1, .func = PUSH },
    { .name = "POP", .args_count = 0, .func = POP }
};

static const size_t instructions_count = sizeof(instructions) / sizeof(instructions[0]);

#endif // INSTRUCTIONS_H_