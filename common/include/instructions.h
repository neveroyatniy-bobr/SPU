#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_

#include <stdio.h>

#include "processor.h"

typedef ProcessorError (*InstructionFunc) (const int* args, Processor* processor);

#define PROCESSOR_STACK_DO_OR_DIE(func, processor)              \
    func;                                                       \
    if (processor->stack.last_error_code != STACK_OK) {         \
        return processor->last_error_code = STACK_ERROR;        \
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

ProcessorError PUSHR(const int* args, Processor* processor);

ProcessorError POPR(const int* args, Processor* processor);

ProcessorError JMP(const int* args, Processor* processor);

ProcessorError JE(const int* args, Processor* processor);

ProcessorError JNE(const int* args, Processor* processor);

ProcessorError JB(const int* args, Processor* processor);

ProcessorError JBE(const int* args, Processor* processor);

ProcessorError JA(const int* args, Processor* processor);

ProcessorError JAE(const int* args, Processor* processor);

ProcessorError COM(const int* /*args*/, Processor* processor);

ProcessorError CALL(const int* args, Processor* processor);

ProcessorError RET(const int* /*args*/, Processor* processor);

ProcessorError PUSHM(const int* args, Processor* processor);

ProcessorError POPM(const int* args, Processor* processor);

ProcessorError PUSHMR(const int* args, Processor* processor);

ProcessorError POPMR(const int* args, Processor* processor);

struct Instruction {
    const char* name;
    const size_t args_count;
    InstructionFunc func;
};

static const Instruction instructions[] = {
    { .name = "PUSH", .args_count = 1, .func = PUSH },
    { .name = "POP", .args_count = 0, .func = POP },
    { .name = "ADD", .args_count = 0, .func = ADD },
    { .name = "SUB", .args_count = 0, .func = SUB },
    { .name = "MUL", .args_count = 0, .func = MUL  },
    { .name = "DIV", .args_count = 0, .func = DIV },
    { .name = "SQRT", .args_count = 0, .func = SQRT },
    { .name = "OUT", .args_count = 0, .func = OUT },
    { .name = "IN", .args_count = 0, .func = IN },
    { .name = "PUSHR", .args_count = 1, .func = PUSHR },
    { .name = "POPR", .args_count = 1, .func = POPR },
    { .name = "JMP", .args_count = 1, .func = JMP }, 
    { .name = "JE", .args_count = 1, .func = JE }, 
    { .name = "JNE", .args_count = 1, .func = JNE }, 
    { .name = "JB", .args_count = 1, .func = JB }, 
    { .name = "JBE", .args_count = 1, .func = JBE }, 
    { .name = "JA", .args_count = 1, .func = JA }, 
    { .name = "JAE", .args_count = 1, .func = JAE },
    { .name = "COM", .args_count = 0, .func = COM },
    { .name = "CALL", .args_count = 1, .func = CALL },
    { .name = "RET", .args_count = 0, .func = RET },
    { .name = "PUSHM", .args_count = 1, .func = PUSHM },
    { .name = "POPM", .args_count = 1, .func = POPM },
    { .name = "PUSHMR", .args_count = 1, .func = PUSHMR },
    { .name = "POPMR", .args_count = 1, .func = POPMR }
};

static const size_t instructions_count = sizeof(instructions) / sizeof(instructions[0]);

#endif // INSTRUCTIONS_H_