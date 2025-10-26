#include "processor.h"

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include "instructions.h"
#include "stack.h"
#include "color.h"
#include "vector.h"
#include "protected_free.h"

//FIXME проуессор не общий файл должен быть

void ProcessorPrintError(ProcessorError error_code) {
    switch (error_code) {
    case PROCESSOR_OK:
        fprintf(stderr, "Выполнено без ошибок\n");
        break;
    case STACK_ERROR:
        fprintf(stderr, "Ошибка стэка\n");
        break;
    case PROCESSOR_HANDLER_NULL_PTR:
        fprintf(stderr, "Нулевой указатель на хэндлер процессора\n");
        break;
    case PROCESSOR_DIV_BY_ZERO_ERROR:
        fprintf(stderr, "Деление на 0\n");
        break;
    default:
        fprintf(stderr, "Непредвиденная ошибка\n");
        break;
    }
}

ProcessorError ProcessorVerefy(Processor* processor) {
    assert(processor != NULL);

    if (processor->last_error_code != PROCESSOR_OK) {
        return processor->last_error_code;
    }

    if (StackVerefy(&processor->stack) != STACK_OK) {
        return processor->last_error_code = STACK_ERROR;
    }

    return processor->last_error_code = PROCESSOR_OK;
}

void ProcessorDump(Processor* processor, const char* file, size_t line) {
    assert(processor != NULL);
    assert(file != NULL);

    printf(BYEL "============PROCESSOR=DUMP============\n" reset);
    StackDump(&processor->stack, file, line);

    for (size_t reg_i = 0; reg_i < REG_COUNT; reg_i++) {
        printf("%d ", processor->regs[reg_i]);
    }
    printf("\n");
}

void ProcessorStdHandler(Processor* processor, const char* file, size_t line) {
    assert(processor != NULL);
    assert(file != NULL);

    ProcessorDump(processor, file, line);
}

ProcessorError ProcessorSetHandler(Processor* processor, ProcessorHandler handler) {
    assert(processor != NULL);

    if (handler == NULL) {
        return processor->last_error_code = PROCESSOR_HANDLER_NULL_PTR;
    }

    processor->handler = handler;

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError ProcessorSetStdHandler(Processor* processor) {
    assert(processor != NULL);

    processor->handler = ProcessorStdHandler;
    return processor->last_error_code = PROCESSOR_OK;
}

bool ProcessorDie(Processor* processor, const char* file, size_t line) {
    assert(processor != NULL);
    assert(file != NULL);

    processor->handler(processor, file, line);
    return 0;
}

ProcessorError ProcessorInit(Processor** processor, const char* bytecode_file_name) {
    assert(processor != NULL);
    assert(bytecode_file_name != NULL);

    Processor* loc_processor = (Processor*)calloc(1, sizeof(Processor));

    StackInit(&loc_processor->stack, 0);

    StackInit(&loc_processor->call_stack, 0);

    ProcessorLoadBytecodeFile(loc_processor, bytecode_file_name);

    memset(loc_processor->regs, 0, sizeof(loc_processor->regs));

    memset(loc_processor->mem, 0, sizeof(loc_processor->mem));

    loc_processor->instruction_ptr = 0;

    loc_processor->handler = ProcessorStdHandler;

    loc_processor->last_error_code = PROCESSOR_OK;
    
    *processor = loc_processor;

    ProcessorCheck((*processor));

    return (*processor)->last_error_code = PROCESSOR_OK;
}

static size_t FileSize(int file) {
    assert(file >= 0);

    struct stat stats = {};

    if (fstat(file, &stats) != 0) {
        fprintf(stderr, "Не удалось прочитать статистику файла. %s\n", strerror(errno));
        return 0;
    }

    return (size_t)stats.st_size;
}

ProcessorError ProcessorLoadBytecodeFile(Processor* processor, const char* bytecode_file_name) {
    assert(processor != NULL);
    assert(bytecode_file_name != NULL);

    int bytecode_file = open(bytecode_file_name, O_RDONLY);

    size_t file_size = FileSize(bytecode_file);

    VectorInit(&processor->program_vec, file_size / sizeof(int), sizeof(int));

    read(bytecode_file, processor->program_vec->data, file_size);

    processor->program_vec->size = file_size / processor->program_vec->elem_size;

    close(bytecode_file);

    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError Process(Processor* processor) {
    assert(processor != NULL);

    for (; processor->instruction_ptr < processor->program_vec->size; processor->instruction_ptr++) {
        int instruction_i = 0;
        VectorGet(processor->program_vec, processor->instruction_ptr, &instruction_i);
        Instruction instruction = instructions[instruction_i];

        int args[MAX_ARGS_COUNT] = {};
        for (size_t i = 0; i < instruction.args_count; i++) {
            VectorGet(processor->program_vec, ++processor->instruction_ptr, &args[i]);
        }

        PROCESSOR_DO_OR_DIE(instruction.func(processor, args), processor);
    }
    return processor->last_error_code = PROCESSOR_OK;
}

ProcessorError ProcessorFree(Processor* processor) {
    ProcessorCheck(processor);

    StackFree(&processor->stack);
    StackFree(&processor->call_stack);
    VectorFree(processor->program_vec);
    protected_free(processor);

    return PROCESSOR_OK;
}