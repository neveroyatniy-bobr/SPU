#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include "stack.h"
#include "int_vector.h"

struct Processor
{
    Stack stack;
    size_t regs[4];
    IntVector program_vec;
    size_t instruction_ptr;
};

enum ProcessorError
{
    PROCESSOR_OK               =  0,
    STACK_ERROR                =  1,
    REGS_NULL_PTR              =  2,
    PROCESSOR_HANDLER_NULL_PTR =  3,
    PROCESSOR_NULL_PTR         =  4
};

/// @brief Выводит сообщение об ошибке процессора в stderr
/// @param error_code Код ошибки
void ProcessorPrintError(ProcessorError error_code);

typedef void (*ProcessorHandler)(Processor* processor, ProcessorError error_code, const char* file, size_t line);

ProcessorError ProcessorVerefy(Processor* processor);

#define ProcessorCheck(processor)                                 \
    {                                                             \
        ProcessorError _error_code = ProcessorVerefy(processor);  \
        if (_error_code != PROCESSOR_OK) {                        \
            return _error_code;                                   \
        }                                                         \
    }                                                             

void ProcessorDump(Processor* processor, ProcessorError error_code, const char* file, size_t line);

void ProcessorStdHandler(Processor* processor, ProcessorError error_code, const char* file, size_t line);

ProcessorError ProcessorSetHandler(ProcessorHandler handler);

ProcessorError ProcessorSetStdHandler();

bool ProcessorDie(Processor* processor, ProcessorError error_code, const char* file, size_t line);

#define PROCESSOR_DO_OR_DIE(func, processor) { ProcessorError err_code_ = PROCESSOR_OK; (PROCESSOR_OK == (err_code_ = func)) || ProcessorDie(processor, err_code_, __FILE__, __LINE__); }

ProcessorError ProcessorInit(Processor** processor);

ProcessorError ProcessorLoadBCFile(Processor* processor, const char* bytecode_file_name);

static size_t FileSize(int file);

ProcessorError Process(Processor* processor);

ProcessorError ProcessorFree(Processor* processor);

#endif // PROCESSOR_H_