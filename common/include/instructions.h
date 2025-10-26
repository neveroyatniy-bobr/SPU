#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_

#include <stdio.h>

#include "processor.h"

typedef ProcessorError (*InstructionFunc) (Processor* processor, const int* args);

#define PROCESSOR_STACK_DO_OR_DIE(func, processor)              \
    func;                                                       \
    if (processor->stack.last_error_code != STACK_OK) {         \
        return processor->last_error_code = STACK_ERROR;        \
    }

/// @brief Добавляет elem в конец стэка
/// @param args { elem }
/// @param processor Процессор в котором выполняется инструкция
/// @return Код ошибки
ProcessorError PUSH(Processor* processor, const int* args);

ProcessorError POP(Processor* processor, const int* /*args*/);

ProcessorError ADD(Processor* processor, const int* /*args*/);

ProcessorError SUB(Processor* processor, const int* /*args*/);

ProcessorError MUL(Processor* processor, const int* /*args*/);

ProcessorError DIV(Processor* processor, const int* /*args*/);

ProcessorError SQRT(Processor* processor, const int* /*args*/);

ProcessorError OUT(Processor* processor, const int* /*args*/);

ProcessorError IN(Processor* processor, const int* /*args*/);

ProcessorError PUSHR(Processor* processor, const int* args);

ProcessorError POPR(Processor* processor, const int* args);

ProcessorError JMP(Processor* processor, const int* args);

ProcessorError JE(Processor* processor, const int* args);

ProcessorError JNE(Processor* processor, const int* args);

ProcessorError JB(Processor* processor, const int* args);

ProcessorError JBE(Processor* processor, const int* args);

ProcessorError JA(Processor* processor, const int* args);

ProcessorError JAE(Processor* processor, const int* args);

ProcessorError COM(Processor* processor, const int* /*args*/);

ProcessorError CALL(Processor* processor, const int* args);

ProcessorError RET(Processor* processor, const int* /*args*/);

ProcessorError PUSHM(Processor* processor, const int* args);

ProcessorError POPM(Processor* processor, const int* args);

ProcessorError PUSHMR(Processor* processor, const int* args);

ProcessorError POPMR(Processor* processor, const int* args);

struct Instruction {
    const char* name;
    const size_t args_count;
    InstructionFunc func;
};

static const Instruction instructions[] = {
    { .name =   "PUSH", .args_count = 1, .func =   PUSH },
    { .name =    "POP", .args_count = 0, .func =    POP },
    { .name =    "ADD", .args_count = 0, .func =    ADD },
    { .name =    "SUB", .args_count = 0, .func =    SUB },
    { .name =    "MUL", .args_count = 0, .func =    MUL },
    { .name =    "DIV", .args_count = 0, .func =    DIV },
    { .name =   "SQRT", .args_count = 0, .func =   SQRT },
    { .name =    "OUT", .args_count = 0, .func =    OUT },
    { .name =     "IN", .args_count = 0, .func =     IN },
    { .name =  "PUSHR", .args_count = 1, .func =  PUSHR },
    { .name =   "POPR", .args_count = 1, .func =   POPR },
    { .name =    "JMP", .args_count = 1, .func =    JMP }, 
    { .name =     "JE", .args_count = 1, .func =     JE }, 
    { .name =    "JNE", .args_count = 1, .func =    JNE }, 
    { .name =     "JB", .args_count = 1, .func =     JB }, 
    { .name =    "JBE", .args_count = 1, .func =    JBE }, 
    { .name =     "JA", .args_count = 1, .func =     JA }, 
    { .name =    "JAE", .args_count = 1, .func =    JAE },
    { .name =    "COM", .args_count = 0, .func =    COM },
    { .name =   "CALL", .args_count = 1, .func =   CALL },
    { .name =    "RET", .args_count = 0, .func =    RET },
    { .name =  "PUSHM", .args_count = 1, .func =  PUSHM },
    { .name =   "POPM", .args_count = 1, .func =   POPM },
    { .name = "PUSHMR", .args_count = 1, .func = PUSHMR },
    { .name =  "POPMR", .args_count = 1, .func =  POPMR }
};

static const size_t MAX_ARGS_COUNT = 1;

static const size_t instructions_count = sizeof(instructions) / sizeof(instructions[0]);

#endif // INSTRUCTIONS_H_