#include "translator.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "text.h"
#include "instructions.h"
#include "vector.h"
#include "labels.h"
#include "define.h"
#include "protected_free.h"
#include "color.h"

// FIXME Нормально разбить на функции
// FIXME Мб разбить на файлы
// FIXME Добавить полную обработку ошибок для все структур с дампами

static bool IsStrInt(const char* str);

TranslatorError TranslatorInit(Translator** translator) {
    assert(translator);
    
    Translator* loc_translator = (Translator*)calloc(1, sizeof(Translator));
    
    if (loc_translator == NULL) {
        return TRANSLATOR_ALLOC_ERROR;
    }

    VectorInit(&loc_translator->program_vec, 0, sizeof(int));

    VectorInit(&loc_translator->labels_vec, 0, sizeof(Label));

    VectorInit(&loc_translator->defines, 0, sizeof(Define));

    memcpy(loc_translator->reg_names, reg_names, sizeof(reg_names));

    TextInit(&loc_translator->program);
    
    loc_translator->last_error_code = TRANSLATOR_OK;

    loc_translator->log = { .cpp_file_name =    "",
                            .cpp_line      =     0,
                            .is_asm_error  = false,
                            .asm_file_name =    "",
                            .asm_line      =     0
                          };

    loc_translator->handler = TranslatorStdHandler;

    *translator = loc_translator;

    TranslatorCheck((*translator));
    
    return (*translator)->last_error_code = TRANSLATOR_OK;
}

TranslatorError TranslatorFree(Translator* translator) {
    assert(translator);

    TranslatorCheck(translator);

    TextMemoryFree(&translator->program);

    VectorFree(translator->program_vec);

    VectorFree(translator->labels_vec);

    VectorFree(translator->defines);

    protected_free(translator);

    return TRANSLATOR_OK;
}


//FIXME добавить обработку с помощью макроса типа if !OK -> return err
TranslatorError Translate(Translator* translator,  const char* asm_file_name, const char* bytecode_file_name) {
    assert(asm_file_name);
    assert(bytecode_file_name);

    TranslatorCheck(translator);

    translator->log.asm_file_name = asm_file_name;

    TextParse(&translator->program, asm_file_name);

    TRANSLATOR_TRANSLATOR_DO_OR_DIE(PredBytecodeConstructor(translator), translator);

    TRANSLATOR_TRANSLATOR_DO_OR_DIE(ProgramVecConstructor(translator), translator);

    TRANSLATOR_TRANSLATOR_DO_OR_DIE(UploadBytecodeFile(translator, bytecode_file_name), translator);

    TranslatorCheck(translator);

    return translator->last_error_code = TRANSLATOR_OK;
}

static bool IsStrInt(const char* str) {
    assert(str);
    
    int dummy_i;
    char dummy_c;

    return sscanf(str, "%d%c", &dummy_i, &dummy_c) == 1;
}

TranslatorError PredBytecodeConstructor(Translator* translator) {
    assert(translator);

    TranslatorCheck(translator);

    int instruction_pointer = 0;

    for (size_t line_i = 0; line_i < translator->program.size; line_i++) {
        Line* line = &translator->program.data[line_i];

        // FIXME фулл кусок ниже для апдейта длинны можно вынести в функцию

        while (isspace(line->data[0]) && line->data[0] != '\n') { // FIXME функция SkipSpaces
            line->data++;
            line->size--;
        }

        char* end_of_line = strchr(line->data, ';');

        size_t new_line_size = (size_t)(end_of_line - line->data);

        if (line->size == 0) {
            continue;
        }

        if (end_of_line == NULL) {
            translator->log.is_asm_error = true;
            translator->log.asm_line = line_i;
            return translator->last_error_code = EXPEXTED_DOT_COMMA;
        }

        *end_of_line = '\0';
        line->size = new_line_size;

        char* instruction_name = strtok(line->data, " ");
        bool is_instruction = false;
        size_t true_args_count = 0;

        for (size_t instruction_i = 0; instruction_i < instructions_count; instruction_i++) {
            if (strcmp(instruction_name, instructions[instruction_i].name) == 0) {
                true_args_count = instructions[instruction_i].args_count;
                is_instruction = true;
                instruction_pointer++;
            }
        }

        size_t args_count = 0;
        while (strtok(NULL, " ") != NULL) { // FIXME - проверить что булет если 2 пробела подряд между аргументами Будет хуйня
            args_count++;
            instruction_pointer++;
        }

        if (instruction_name[0] == LABEL_MARK) {
            if (args_count > 0) {
                translator->log.is_asm_error = true;
                translator->log.asm_line = line_i;
                return translator->last_error_code = LABEL_HAS_NOT_ARGS;
            }

            const char* label_name = instruction_name + 1;
            int label_adress = instruction_pointer;

            Label label = { .name = label_name, .adresses = label_adress };

            VectorPush(translator->labels_vec, &label);

            continue;
        }

        if (strcmp(instruction_name, DEFINE_WORD) == 0) {
            // FIXME - Function
            if (args_count != 2) {
                translator->log.is_asm_error = true;
                translator->log.asm_line = line_i;
                return translator->last_error_code = DEFINES_ARGS_COUNT_ERROR;
            }

            instruction_pointer -= 2;

            char* defined_name = strchr(line->data, '\0') + 1;
            char* define_value = strchr(defined_name, '\0') + 1;

            Define define = {.defined_name = defined_name, .define_value = define_value};

            VectorPush(translator->defines, &define);

            continue;
        }

        if (args_count != true_args_count) {
            translator->log.is_asm_error = true;
            translator->log.asm_line = line_i;
            return translator->last_error_code = INSTRUCTION_ARGS_COUNT_ERROR;
        }

        if (!is_instruction) {
            translator->log.is_asm_error = true;
            translator->log.asm_line = line_i;
            return translator->last_error_code = UNDEFINED_INSTRICTION;
        }
    }

    TranslatorCheck(translator);

    return translator->last_error_code = TRANSLATOR_OK;
}

// FIXME пожалйста пожалйста называй нормально ЕБАНЫЕ функциииии
TranslatorError ProgramVecConstructor(Translator* translator) {
    TranslatorCheck(translator);

    assert(translator);

    for (size_t line_i = 0; line_i < translator->program.size; line_i++) {
        Line line = translator->program.data[line_i];

        char* instruction_name = line.data;

        for (size_t instruction_i = 0; instruction_i < instructions_count; instruction_i++) {
            if (strcmp(instruction_name, instructions[instruction_i].name) == 0) {
                VectorPush(translator->program_vec, &instruction_i);
            }
        }

        char* arg_ptr = strchr(instruction_name, '\0') + 1;


        while (arg_ptr - line.data < (ssize_t)line.size) {
            int define_num = -1;

            //FIXME функция которая ищет дефайн
            for (int define_i = 0; define_i < (int)translator->defines->size; define_i++) {
                Define define = {};
                VectorGet(translator->defines, (size_t)define_i, &define);

                if (strcmp(arg_ptr, define.define_value) == 0) {
                    define_num = define_i;
                }
            }

            char* arg = arg_ptr;
            if (define_num != -1) {
                Define define = {};
                VectorGet(translator->defines, (size_t)define_num, &define);

                arg = define.defined_name;
            }

            if (arg[0] == LABEL_MARK) {
                LabelsArgs(translator, arg);
            }
            else if (arg[0] == REG_START_CHAR && strcmp(instruction_name, DEFINE_WORD) != 0) {
                int reg_num = -1;

                //FIXME функция которая ищет регистр
                for (int reg_i = 0; reg_i < (int)REG_COUNT; reg_i++) {
                    if (strcmp(arg, translator->reg_names[reg_i]) == 0) {
                        reg_num = reg_i;
                    }
                }

                if (reg_num == -1) {
                    translator->log.is_asm_error = true;
                    translator->log.asm_line = line_i;
                    return translator->last_error_code = UNDEFINED_REG;
                }

                VectorPush(translator->program_vec, &reg_num);
            }
            else if (IsStrInt(arg)) {
                int int_arg = atoi(arg);
                VectorPush(translator->program_vec, &int_arg);
            }  
            else if (strcmp(instruction_name, DEFINE_WORD) == 0) { }
            else {
                translator->log.is_asm_error = true;
                translator->log.asm_line = line_i;
                return translator->last_error_code = INCORRECT_ARG;
            }
            arg_ptr = strchr(arg_ptr, '\0') + 1;
        }
    }

    TranslatorCheck(translator);

    return translator->last_error_code = TRANSLATOR_OK;
}

TranslatorError UploadBytecodeFile(Translator* translator, const char* bytecode_file_name) {
    assert(translator);
    assert(bytecode_file_name);

    TranslatorCheck(translator);

    FILE* bytecode_file = fopen(bytecode_file_name, "w");
    if (bytecode_file == NULL) {
        return translator->last_error_code = CREATE_BYTECODE_FILE_ERRROR;
    }

    fwrite(translator->program_vec->data, (size_t)translator->program_vec->elem_size, (size_t)translator->program_vec->size, bytecode_file);

    fclose(bytecode_file);

    TranslatorCheck(translator);

    return translator->last_error_code = TRANSLATOR_OK;
}

char* BytecodeFileName(char* asm_file_name) {
    assert(asm_file_name);

    size_t asm_file_name_len = strlen(asm_file_name);
    char* bytecode_file_name = (char*)calloc(asm_file_name_len + sizeof(".vovalox"), 1);
    strcpy(bytecode_file_name, asm_file_name);
    strcpy(bytecode_file_name + asm_file_name_len, ".vovalox");

    return bytecode_file_name;
}

TranslatorError BytecodeFileFree(char* bytecode_file_name) {
    protected_free(bytecode_file_name);
    return TRANSLATOR_OK;
}

//FIXME Добавить if !OK return err;
TranslatorError LabelsArgs(Translator* translator, const char* arg_ptr) {
    TranslatorCheck(translator);

    for (size_t label_i = 0; label_i < translator->labels_vec->size; label_i++) {
        Label label = {};
        VectorGet(translator->labels_vec, label_i, &label);

        const char* label_name = label.name;
        
        if (strcmp(arg_ptr + 1, label_name) == 0) {
            VectorPush(translator->program_vec, &label.adresses);
        }
    }

    TranslatorCheck(translator);

    return translator->last_error_code =TRANSLATOR_OK;
}

void TranslatorPrintError(TranslatorError error) {
    switch (error) {
    case TRANSLATOR_OK:
        fprintf(stderr, "Выполнено без ошибок\n");
        break;
    case TRANSLATOR_HANDLER_NULL_PTR:
        fprintf(stderr, "Нулевой указатель на хэндлер транслятора\n");
        break;
    case DEFINES_VECTOR_ERROR:
        fprintf(stderr, "Ошибка вектора с дефайнами\n");
        break;
    case LABELS_VECTOR_ERROR:
        fprintf(stderr, "Ошибка вектора с метками\n");
        break;
    case PROGRAM_VECTOR_ERROR:
        fprintf(stderr, "Ошибка вектора с байткодом\n");
        break;
    case PROGRAM_TEXT_ERROR: 
        fprintf(stderr, "Ошибка в тексте программы\n");
        break;
    case TRANSLATOR_ALLOC_ERROR:
        fprintf(stderr, "Не удалось выделить память на транслятор\n");
        break;
    case EXPEXTED_DOT_COMMA:
        fprintf(stderr, "Пропущена ;\n");
        break;
    case LABEL_HAS_NOT_ARGS:
        fprintf(stderr, "У метки не может быть аргументов\n");
        break;
    case DEFINES_ARGS_COUNT_ERROR:
        fprintf(stderr, "Неверное количество аргументов у define\n");
        break;
    case INSTRUCTION_ARGS_COUNT_ERROR:
        fprintf(stderr, "Неверное количество аргументов у инструкции\n");
        break;
    case UNDEFINED_INSTRICTION:
        fprintf(stderr, "Несуществующая инструкция\n");
        break;
    case UNDEFINED_REG:
        fprintf(stderr, "Неверное имя регистра\n");
        break;
    case INCORRECT_ARG:
        fprintf(stderr, "Неверный аргумент\n");
        break;
    case CREATE_BYTECODE_FILE_ERRROR:
        fprintf(stderr, "Не удалось создать байткод файл.\n");
        break;
    default:
        fprintf(stderr, "Непредвиденная ошибка\n");
        break;
    }
}

TranslatorError TranslatorVerefy(Translator* translator){
    assert(translator);

    if (translator->last_error_code != TRANSLATOR_OK) {
        return translator->last_error_code;
    }

    if (VectorVerefy(translator->defines) != VECTOR_OK) {
        return translator->last_error_code = DEFINES_VECTOR_ERROR;
    }

    if (VectorVerefy(translator->labels_vec) != VECTOR_OK) {
        return translator->last_error_code = LABELS_VECTOR_ERROR;
    }

    if (VectorVerefy(translator->program_vec) != VECTOR_OK) {
        return translator->last_error_code = PROGRAM_VECTOR_ERROR;
    }

    if (translator->handler == NULL) {
        return translator->last_error_code = TRANSLATOR_HANDLER_NULL_PTR;
    }

    return translator->last_error_code = TRANSLATOR_OK;
}

void TranslatorDump(Translator* translator) {
    fprintf(stderr, BYEL "==============TRANSLATOR=DUMP==============\n" reset);

    const char* file;
    size_t line;
    if (translator->log.is_asm_error) {
        file = translator->log.asm_file_name;
        line = translator->log.asm_line;
    } else {
        file = translator->log.cpp_file_name;
        line = translator->log.cpp_line;
    }

    fprintf(stderr, BRED "ERROR in %s:%lu:\n", file, line);
    TranslatorPrintError(translator->last_error_code);
    fprintf(stderr, reset);

    VectorDump(translator->program_vec, file, line);

    VectorDump(translator->labels_vec, file, line);

    VectorDump(translator->defines, file, line);

    fprintf(stderr, BYEL "REG_NAMES:\n" reset);
    for (size_t reg_i = 0; reg_i < REG_COUNT; reg_i++) {
        fprintf(stderr, GRN "reg_names[%lu]" BYEL " = " MAG "%s\n" reset, reg_i, translator->reg_names[reg_i]);
    }

    TextDump(&translator->program, file, (size_t)line);

    fprintf(stderr, GRN "handler_ptr " BYEL "= " MAG "%p\n" reset, translator->handler);
}

void TranslatorStdHandler(Translator* translator) {
    TranslatorDump(translator);
    abort();
}

TranslatorError TranslatorSetHandler(Translator* translator, TranslatorHandler handler) {
    assert(translator);
    assert(handler);

    translator->handler = handler;

    return translator->last_error_code = TRANSLATOR_OK;
}

TranslatorError TranslatorSetStdHandler(Translator* translator) {
    return TranslatorSetHandler(translator, TranslatorStdHandler);
}

bool TranslatorDie(Translator* translator) {
    translator->handler(translator);
    return 0;
}