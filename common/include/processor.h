#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include "stack.h"
#include "vector.h"

static const size_t REG_COUNT = 8;
static const size_t MEM_SIZE = 1024;

struct Processor;

enum ProcessorError
{
    PROCESSOR_OK                =  0,
    STACK_ERROR                 =  1,
    PROCESSOR_HANDLER_NULL_PTR  =  2,
    PROCESSOR_DIV_BY_ZERO_ERROR =  3
};

typedef void (*ProcessorHandler)(Processor* processor, const char* file, size_t line);

struct Processor
{
    Stack stack;
    Stack call_stack;
    int regs[REG_COUNT];
    int mem[MEM_SIZE];
    Vector* program_vec;
    size_t instruction_ptr;
    ProcessorError last_error_code;
    ProcessorHandler handler;
};

/// @brief Выводит сообщение об ошибке процессора в stderr
/// @param error_code Код ошибки
void ProcessorPrintError(ProcessorError error_code);

ProcessorError ProcessorVerefy(Processor* processor);

#define ProcessorCheck(processor)                             \
    processor->last_error_code = ProcessorVerefy(processor);  \
    if (processor->last_error_code != PROCESSOR_OK) {         \
        return processor->last_error_code;                    \
    }                                                         \

void ProcessorDump(Processor* processor, const char* file, size_t line);

void ProcessorStdHandler(Processor* processor, const char* file, size_t line);

ProcessorError ProcessorSetHandler(Processor* processor, ProcessorHandler handler);

ProcessorError ProcessorSetStdHandler(Processor* processor);

bool ProcessorDie(Processor* processor, const char* file, size_t line);

#define PROCESSOR_DO_OR_DIE(func, processor) (PROCESSOR_OK == func) || ProcessorDie(processor, __FILE__, __LINE__);

ProcessorError ProcessorInit(Processor** processor, const char* bytecode_file_name);

ProcessorError ProcessorLoadBytecodeFile(Processor* processor, const char* bytecode_file_name);

ProcessorError Process(Processor* processor);

ProcessorError ProcessorFree(Processor* processor);

#endif // PROCESSOR_H_