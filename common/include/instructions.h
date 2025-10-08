#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_

#include <stdio.h>

#include "processor.h"

typedef ProcessorError (*InstructionFunc) (const int* args, Processor* processor);

/// @brief Добавляет elem в конец стэка
/// @param args { elem }
/// @param processor Процессор в котором выполняется инструкция
/// @return Код ошибки
ProcessorError PUSH(const int* args, Processor* processor);

ProcessorError POP(const int* /*args*/, Processor* processor);

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