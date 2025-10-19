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

// FIXME Нормально разбить на функции
// FIXME Мб разбить на файлы
// FIXME Добавить полную обработку ошибок для все структур с дампами

static bool IsStrInt(const char* str);

void TranslatorInit(Translator** translator) {
    assert(translator);
    
    Translator* loc_translator = (Translator*)calloc(1, sizeof(Translator));
    
    if (loc_translator == NULL) {
        fprintf(stderr, "Не удалось выделить память на транслятор\n");
        abort();
    }

    VectorInit(&loc_translator->program_vec, 0, sizeof(int));

    VectorInit(&loc_translator->labels_vec, 0, sizeof(Label));

    char reg_names[REG_COUNT][REG_NAME_MAX_SIZE] = { "RAX", "RBX", "RCX", "RDX", "REX", "RFX", "RGX", "RHX" };

    memcpy(loc_translator->reg_names, reg_names, sizeof(reg_names));

    loc_translator->program = {};

    *translator = loc_translator;
}

void TranslatorFree(Translator* translator) {
    assert(translator);

    TextMemoryFree(translator->program);

    VectorFree(translator->program_vec);

    VectorFree(translator->labels_vec);

    free(translator);
}

void Translate(const char* asm_file_name, const char* bytecode_file_name) {
    assert(asm_file_name);
    assert(bytecode_file_name);

    Translator* translator = NULL;

    TranslatorInit(&translator);

    TextParse(&translator->program, asm_file_name);

    PredBytecodeConstructor(translator);

    ProgramVecConstructor(translator);

    UploadBytecodeFile(translator, bytecode_file_name);
    
    TranslatorFree(translator);
}

static bool IsStrInt(const char* str) {
    assert(str);
    
    int dummy_i;
    char dummy_c;

    return sscanf(str, "%d%c", &dummy_i, &dummy_c) == 1;
}

void PredBytecodeConstructor(Translator* translator) {
    assert(translator);

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
            fprintf(stderr, "Пропущена ;\n");
            printf("line: %lu\n", line_i + 1);
            return;
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
        while (strtok(NULL, " ") != NULL) { // FIXME - проверить что булет если 2 пробела подряд между аргументами
            args_count++;
            instruction_pointer++;
        }

        if (instruction_name[0] == LABEL_MARK) {
            if (args_count > 0) {
                fprintf(stderr, "У метки не может быть аргументов\n");
                printf("line: %lu\n", line_i + 1);
                return;
            }

            const char* label_name = instruction_name + 1;
            int label_adress = instruction_pointer;

            Label label = { .name = label_name, .adresses = label_adress };

            VectorPush(translator->labels_vec, &label);

            continue;
        }

        if (strcmp(instruction_name, "ALIAS") == 0) { // FIXME - const str
            // FIXME - Function
            if (args_count != 2) {
                fprintf(stderr, "Неверное количество аргументов у ALIAS\n");
                printf("line: %lu\n", line_i + 1);
                return;
            }

            instruction_pointer -= 2;

            char* defined_name = strchr(line->data, '\0') + 1;
            char* defined_name_value = strchr(defined_name, '\0') + 1;

            if (strlen(defined_name_value) + 1 > 32) { // FIXME - const
                fprintf(stderr, "Слишком длинное имя регистра\n");
                printf("line: %lu\n", line_i + 1);
                return;
            }


            for (size_t reg_i = 0; reg_i < 8; reg_i++) { // FIXME - const
                if (strcmp(defined_name, translator->reg_names[reg_i]) == 0) {
                    strncpy(translator->reg_names[reg_i], defined_name_value, 32);
                }
            }

            if (defined_name_value[0] != 'R') { // FIXME - const
                fprintf(stderr, "Имя регистра обязательно должно начинаться с 'R'\n");
                printf("line: %lu\n", line_i + 1);
                return;
            }

            continue;
        }

        if (args_count != true_args_count) {
            fprintf(stderr, "Неверное количество аргументов у инструкции\n");
            printf("line: %lu\n", line_i + 1);
            return;
        }

        if (!is_instruction) {
            fprintf(stderr, "Несуществующая инструкция: %s\n", instruction_name);
            printf("line: %lu\n", line_i + 1);
            return;
        }
    
    }
}

// FIXME пожалйста пожалйста называй нормально ЕБАНЫЕ функциииии
void ProgramVecConstructor(Translator* translator) {
    assert(translator);

    for (size_t line_i = 0; line_i < translator->program.size; line_i++) {
        Line line = translator->program.data[line_i];

        char* instruction_name = line.data;

        for (size_t instruction_i = 0; instruction_i < instructions_count; instruction_i++) {
            if (strcmp(instruction_name, instructions[instruction_i].name) == 0) {
                VectorPush(translator->program_vec, &instruction_i);
            }
        }

        char* arg = strchr(instruction_name, '\0') + 1;
        while (arg - line.data < (ssize_t)line.size) {
            if (arg[0] == LABEL_MARK) {
                LabelsArgs(translator, arg);
            }
            else if (arg[0] == 'R' && strcmp(instruction_name, "ALIAS") != 0) { // FIXME - const
                int reg_num = -1;

                //FIXME функция которая ищет регистр
                for (int reg_i = 0; reg_i < 8; reg_i++) { //FIXME const
                    if (strcmp(arg, translator->reg_names[reg_i]) == 0) {
                        reg_num = reg_i;
                    }
                }

                if (reg_num == -1) {
                    fprintf(stderr, "Неверное имя регистра\n");
                    printf("line: %lu\n", line_i + 1);
                    return;
                }

                VectorPush(translator->program_vec, &reg_num);
            }
            else if (IsStrInt(arg)) {
                int int_arg = atoi(arg);
                VectorPush(translator->program_vec, &int_arg);
            }  
            else if (strcmp(instruction_name, "ALIAS") == 0) { }
            else {
                fprintf(stderr, "Неверный аргумент\n");
                printf("line: %lu\n", line_i + 1);
                return;
            }
            arg = strchr(arg, '\0') + 1;
        }
    }
}

void UploadBytecodeFile(Translator* translator, const char* bytecode_file_name) {
    assert(translator);
    assert(bytecode_file_name);

    FILE* bytecode_file = fopen(bytecode_file_name, "w");
    if (bytecode_file == NULL) {
        fprintf(stderr, "Не удалось создать файл. %s\n", strerror(errno));
        return;
    }

    fwrite(translator->program_vec->data, (size_t)translator->program_vec->elem_size, (size_t)translator->program_vec->size, bytecode_file);

    fclose(bytecode_file);
}

char* BytecodeFileName(char* asm_file_name) {
    assert(asm_file_name);

    size_t asm_file_name_len = strlen(asm_file_name);
    char* bytecode_file_name = (char*)calloc(asm_file_name_len + sizeof(".vovalox"), 1);
    strcpy(bytecode_file_name, asm_file_name);
    strcpy(bytecode_file_name + asm_file_name_len, ".vovalox");

    return bytecode_file_name;
}

void LabelsArgs(Translator* translator, const char* arg) {
    for (size_t label_i = 0; label_i < translator->labels_vec->size; label_i++) {
        Label label = {};
        VectorGet(translator->labels_vec, label_i, &label);

        const char* label_name = label.name;
        
        if (strcmp(arg + 1, label_name) == 0) {
            VectorPush(translator->program_vec, &label.adresses);
        }
    }
}