#include "processor.h"

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include "instructions.h"
#include "stack.h"
#include "int_vector.h"
#include "color.h"

void ProcessorPrintError(ProcessorError error_code) {
    switch (error_code)
    {
    case PROCESSOR_OK:
        fprintf(stderr, "Выполнено без ошибок\n");
        break;
    case STACK_ERROR:
        fprintf(stderr, "Ошибка стэка\n");
        break;
    case PROCESSOR_HANDLER_NULL_PTR:
        fprintf(stderr, "Нулевой указатель на хэндлер процессора\n");
        break;
    case PROCESSOR_NULL_PTR:
        fprintf(stderr, "Нулевой указатель на процессор\n");
        break;
    default:
        fprintf(stderr, "Непредвиденная ошибка\n");
        break;
    }
}

ProcessorError ProcessorVerefy(Processor* processor)
{
    if (processor == NULL) {
        return processor->last_error_code = PROCESSOR_NULL_PTR;
    }

    if (processor->last_error_code != PROCESSOR_OK) {
        return processor->last_error_code;
    }

    if (StackVerefy(&processor->stack) != STACK_OK) {
        return processor->last_error_code = STACK_ERROR;
    }

    return processor->last_error_code = PROCESSOR_OK;
}

void ProcessorDump(Processor* processor, const char* file, size_t line) {
    printf(BYEL "============PROCESSOR=DUMP============\n" reset);
    StackDump(&processor->stack, file, line);
}

void ProcessorStdHandler(Processor* processor, const char* file, size_t line) {
    ProcessorDump(processor, file, line);
}

ProcessorError ProcessorSetHandler(Processor* processor, ProcessorHandler handler) {
    if (handler == NULL) {
        return processor->last_error_code = PROCESSOR_HANDLER_NULL_PTR;
    }

    processor->handler = handler;

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError ProcessorSetStdHandler(Processor* processor) {
    processor->handler = ProcessorStdHandler;
    return processor->last_error_code = PROCESSOR_OK;
}

bool ProcessorDie(Processor* processor, const char* file, size_t line) {
    processor->handler(processor, file, line);
    return 0;
}

ProcessorError ProcessorInit(Processor** processor, const char* bytecode_file_name) {
    *processor = (Processor*)calloc(1, sizeof(Processor));

    StackInit(&(*processor)->stack, 0);
    StackInit(&(*processor)->call_stack, 0);
    ProcessorLoadBCFile(*processor, bytecode_file_name);
    for (size_t reg_i = 0; reg_i < sizeof(&(*processor)->regs) / sizeof(&(*processor)->regs[0]); reg_i++) {
        (*processor)->regs[reg_i] = 0;
    }
    (*processor)->instruction_ptr = 0;

    (*processor)->handler = ProcessorStdHandler;

    (*processor)->last_error_code = PROCESSOR_OK;

    ProcessorCheck((*processor));

    return (*processor)->last_error_code = PROCESSOR_OK;
}

static size_t FileSize(int file) {
    struct stat stats = {};

    if (fstat(file, &stats) != 0) {
        fprintf(stderr, "Не удалось прочитать статистику файла. %s\n", strerror(errno));
        return 0;
    }

    return (size_t)stats.st_size;
}

ProcessorError ProcessorLoadBCFile(Processor* processor, const char* bytecode_file_name) {
    int bytecode_file = open(bytecode_file_name, O_RDONLY);

    size_t file_size = FileSize(bytecode_file);

    IntVectorInit(&processor->program_vec, file_size);

    read(bytecode_file, processor->program_vec.data, file_size);

    processor->program_vec.size = file_size / sizeof(processor->program_vec.data[0]);

    close(bytecode_file);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError Process(Processor* processor) {
    for (; processor->instruction_ptr < processor->program_vec.size; processor->instruction_ptr++) {
        Instruction instruction = instructions[processor->program_vec.data[processor->instruction_ptr]];

        int* args = (int*)calloc(sizeof(instruction.args_count), sizeof(int));
        for (size_t i = 0; i < instruction.args_count; i++) {
            args[i] = processor->program_vec.data[++processor->instruction_ptr];
        }

        PROCESSOR_DO_OR_DIE(instruction.func(args, processor), processor);

        free(args);
    }
    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError ProcessorFree(Processor* processor) {
    ProcessorCheck(processor);

    if (processor == NULL) {
        return processor->last_error_code = PROCESSOR_NULL_PTR;
    }

    StackFree(&processor->stack);
    StackFree(&processor->call_stack);
    IntVectorFree(&processor->program_vec);
    free(processor);

    return PROCESSOR_OK;
}