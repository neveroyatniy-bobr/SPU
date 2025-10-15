#include "translator.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "text.h"
#include "instructions.h"
#include "int_vector.h"
#include "labels_vec.h"

// FIXME Нормально разбить на функции
// FIXME Мб разбить на файлы
// FIXME Добавить полную обработку ошибок для все структур с дампами

void TranslatorInit(Translator** translator) {
    *translator = (Translator*)calloc(1, sizeof(Translator));
    //FIXME - NULL

    IntVectorInit(&(*translator)->program_vec, 0);

    LabelsVecInit(&(*translator)->labels_vec);

    char reg_names[REG_COUNT][REG_NAME_MAX_SIZE] = { "RAX", "RBX", "RCX", "RDX", "REX", "RFX", "RGX", "RHX" };

    memcpy((*translator)->reg_names, reg_names, sizeof(reg_names));

    (*translator)->program = {};
}

void TranslatorFree(Translator* translator) {
    TextMemoryFree(translator->program);

    IntVectorFree(&translator->program_vec);

    LabelsVecFree(&translator->labels_vec);

    free(translator);
}

void Translate(const char* asm_file_name, const char* bytecode_file_name) {
    Translator* translator = NULL;

    TranslatorInit(&translator);

    TextParse(&translator->program, asm_file_name);

    PredBytecodeConstructor(translator);

    ProgramVecConstructor(translator);

    UploadBytecodeFile(translator, bytecode_file_name);
    
    TranslatorFree(translator);
}

static bool IsStrInt(const char* str) {
    int i;
    char c;

    if (sscanf(str, "%d%1s", &i, &c) == 1) {
        return true;
    }

    return false;
}

void PredBytecodeConstructor(Translator* translator) {
    int instruction_pointer = 0;

    for (size_t line_i = 0; line_i < translator->program.size; line_i++) {
        Line* line = &translator->program.data[line_i];

        while (line->data[0] == ' ') { // FIXME - isspace
            line->data++;
            line->size--;
        }

        char* end_of_line = strchr(line->data, ';');

        size_t new_line_size = (size_t)(end_of_line - line->data);

        // FIXME - inverse condition
        if (line->size != 0) {
            if (end_of_line == NULL) {
                fprintf(stderr, "Пропущена ;\n");
                printf("line: %lu\n", line_i + 1);
                return;
            }

            *end_of_line = '\0';
            line->size = new_line_size;

            char* instruction_name = NULL;
            instruction_name = strtok(line->data, " ");
            bool is_instruction = false;
            size_t true_args_count;

            for (size_t instruction_i = 0; instruction_i < instructions_count; instruction_i++) {
                if (strcmp(instruction_name, instructions[instruction_i].name) == 0) {
                    true_args_count = instructions[instruction_i].args_count;
                    is_instruction = true;
                    instruction_pointer++;
                }
            }

            char* arg = NULL;
            size_t args_count = 0;
            while ((arg = strtok(NULL, " ")) != NULL) {
                args_count++;
                instruction_pointer++;
            }

            if (instruction_name[0] == ':') {
                is_instruction = true;

                if (args_count > 0) {
                    fprintf(stderr, "У метки не может быть аргументов\n");
                    printf("line: %lu\n", line_i + 1);
                    return;
                }

                Line label_name = { instruction_name + 1, strlen(instruction_name + 1) };
                int label_adress = instruction_pointer;
                LabelsVecAdd(&translator->labels_vec, label_name, label_adress);
            }
            else if (strcmp(instruction_name, "ALIAS") == 0) { // FIXME - const str
                // FIXME - Function
                true_args_count = 2;
                is_instruction = true;

                if (args_count != 2) {
                    fprintf(stderr, "Неверное количество аргументов у ALIAS\n");
                    printf("line: %lu\n", line_i + 1);
                    return;
                }

                instruction_pointer -= 2;

                char* reg_name = strchr(line->data, '\0') + 1;
                char* new_reg_name = strchr(reg_name, '\0') + 1;

                if (strlen(new_reg_name) + 1 > 32) { // FIXME - const
                    fprintf(stderr, "Слишком длинное имя регистра\n");
                    printf("line: %lu\n", line_i + 1);
                    return;
                }


                for (size_t reg_i = 0; reg_i < 8; reg_i++) { // FIXME - const
                    if (strcmp(reg_name, translator->reg_names[reg_i]) == 0) {
                        strncpy(translator->reg_names[reg_i], new_reg_name, 32);
                    }
                }

                if (new_reg_name[0] != 'R') { // FIXME - const
                    fprintf(stderr, "Имя регистра обязательно должно начинаться с 'R'\n");
                    printf("line: %lu\n", line_i + 1);
                    return;
                }
            }
            else {
                if (args_count != true_args_count) {
                    fprintf(stderr, "Неверное количество аргументов у инструкции\n");
                    printf("line: %lu\n", line_i + 1);
                    return;
                }
            }

            if (!is_instruction) {
                fprintf(stderr, "Несуществующая инструкция: %s\n", instruction_name);
                printf("line: %lu\n", line_i + 1);
                return;
            }
        }
    }
}

void ProgramVecConstructor(Translator* translator) {
    for (size_t line_i = 0; line_i < translator->program.size; line_i++) {
        Line line = translator->program.data[line_i];

        char* instruction_name = line.data;

        for (size_t instruction_i = 0; instruction_i < instructions_count; instruction_i++) {
            if (strcmp(instruction_name, instructions[instruction_i].name) == 0) {
                IntVectorAdd(&translator->program_vec, (int)instruction_i);
            }
        }

        char* arg = strchr(instruction_name, '\0') + 1;
        while (arg - line.data < (ssize_t)line.size) {
            if (arg[0] == ':') { // FIXME - const
                for (size_t label_i = 0; label_i < translator->labels_vec.names.size; label_i++) {
                    const char* label_name = translator->labels_vec.names.data[label_i].data;
                    if (strcmp(arg + 1, label_name) == 0) {
                        IntVectorAdd(&translator->program_vec, translator->labels_vec.adresses.data[label_i]);
                    }
                }
            }
            else if (arg[0] == 'R' && strcmp(instruction_name, "ALIAS") != 0) { // FIXME - const
                int reg_num = -1;

                for (int reg_i = 0; reg_i < 8; reg_i++) {
                    if (strcmp(arg, translator->reg_names[reg_i]) == 0) {
                        reg_num = reg_i;
                    }
                }

                if (reg_num == -1) {
                    fprintf(stderr, "Неверное имя регистра\n");
                    printf("line: %lu\n", line_i + 1);
                    return;
                }

                IntVectorAdd(&translator->program_vec, reg_num);
            }
            else if (IsStrInt(arg)) {
                IntVectorAdd(&translator->program_vec, atoi(arg));
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
    FILE* bytecode_file = fopen(bytecode_file_name, "w");
    if (bytecode_file == NULL) {
        fprintf(stderr, "Не удалось создать файл. %s\n", strerror(errno));
        return;
    }

    fwrite(translator->program_vec.data, sizeof(translator->program_vec.data[0]), translator->program_vec.size, bytecode_file);

    fclose(bytecode_file);
}

char* BytecodeFileName(char* asm_file_name) {
    size_t asm_file_name_len = strlen(asm_file_name);
    char* bytecode_file_name = (char*)calloc(asm_file_name_len + sizeof(".vovalox"), 1);
    strcpy(bytecode_file_name, asm_file_name);
    strcpy(bytecode_file_name + asm_file_name_len, ".vovalox");

    return bytecode_file_name;
}