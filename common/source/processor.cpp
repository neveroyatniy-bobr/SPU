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
    case REGS_NULL_PTR:
        fprintf(stderr, "Нулевой указатеель на массив регистров\n");
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

static ProcessorHandler HANDLER = ProcessorStdHandler;

ProcessorError ProcessorVerefy(Processor* processor)
{
    if (processor == NULL) {
        return PROCESSOR_NULL_PTR;
    }

    StackError stack_error_code = STACK_OK;
    if ((stack_error_code = StackVerefy(&processor->stack)) != STACK_OK)
    {
        return STACK_ERROR;
    }

    if (processor->regs == NULL)
    {
        return REGS_NULL_PTR;
    }

    return PROCESSOR_OK;
}

void ProcessorDump(Processor* processor, ProcessorError error_code, const char* file, size_t line) {
    printf(BYEL "============PROCESSOR=DUMP============\n" reset);
    StackDump(&processor->stack, StackVerefy(&processor->stack), file, line);
}

void ProcessorStdHandler(Processor* processor, ProcessorError error_code, const char* file, size_t line) {
    ProcessorDump(processor, error_code, file, line);
}

ProcessorError ProcessorSetHandler(ProcessorHandler handler) {
    if (handler == NULL) {
        return PROCESSOR_HANDLER_NULL_PTR;
    }

    HANDLER = handler;

    return PROCESSOR_OK;
}

ProcessorError ProcessorSetStdHandler() {
    HANDLER = ProcessorStdHandler;
    return PROCESSOR_OK;
}

bool ProcessorDie(Processor* processor, ProcessorError error_code, const char* file, size_t line) {
    HANDLER(processor, error_code, file, line);
    return 0;
}

ProcessorError ProcessorInit(Processor** processor) {
    *processor = (Processor*)calloc(1, sizeof(Processor));

    StackInit(&(*processor)->stack, 0);
    ProcessorLoadBCFile(*processor, "../program/asm.vovalox");
    for (int reg_i = 0; reg_i < sizeof(&(*processor)->regs) / sizeof(&(*processor)->regs[0]); reg_i++) {
        (*processor)->regs[reg_i] = 0;
    }
    (*processor)->instruction_ptr = 0;

    ProcessorCheck(*processor);

    return PROCESSOR_OK;
}

ProcessorError ProcessorLoadBCFile(Processor* processor, const char* bytecode_file_name) {
    int bytecode_file = open(bytecode_file_name, O_RDONLY);

    size_t file_size = FileSize(bytecode_file);

    IntVectorInit(&processor->program_vec, file_size);

    read(bytecode_file, processor->program_vec.data, file_size);

    processor->program_vec.size = file_size / sizeof(processor->program_vec.data[0]);

    close(bytecode_file);

    return PROCESSOR_OK;
}

static size_t FileSize(int file) {
    struct stat stats = {};

    if (fstat(file, &stats) != 0) {
        fprintf(stderr, "Не удалось прочитать статистику файла. %s\n", strerror(errno));
        return 0;
    }

    return (size_t)stats.st_size;
}

ProcessorError Process(Processor* processor) {
    for (processor->instruction_ptr; processor->instruction_ptr < processor->program_vec.size; processor->instruction_ptr++) {
        Instruction instruction = instructions[processor->program_vec.data[processor->instruction_ptr]];

        int* args = (int*)calloc(sizeof(instruction.args_count), sizeof(int));
        for (size_t i = 0; i < instruction.args_count; i++) {
            args[i] = processor->program_vec.data[++processor->instruction_ptr];
        }

        PROCESSOR_DO_OR_DIE(instruction.func(args, processor), processor);

        free(args);
    }
    return PROCESSOR_OK;
}

ProcessorError ProcessorFree(Processor* processor) {
    ProcessorCheck(processor);

    if (processor == NULL) {
        return PROCESSOR_NULL_PTR;
    }

    StackFree(&processor->stack);
    IntVectorFree(&processor->program_vec);
    free(processor);

    return PROCESSOR_OK;
}