#include "translator.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "text.h"
#include "instructions.h"
#include "int_vector.h"

void Translate(const char* asm_file_name, const char* bytecode_file_name) {
    IntVector program_vec = {};
    IntVectorInit(&program_vec, 0);

    IntVector label_vec = {};
    IntVectorInit(&label_vec, 0);

    Text program = {};
    TextParse(&program, asm_file_name);

    PredBytecodeConstructor(&program, &label_vec);

    ProgramVecConstructor(&program, &program_vec, &label_vec);

    TextMemoryFree(program);

    UploadBytecodeFile(program_vec, bytecode_file_name);

    IntVectorFree(&program_vec);

    IntVectorFree(&label_vec);
}

void PredBytecodeConstructor(Text* program, IntVector* label_vec) {
    int instruction_pointer = 0;

    for (size_t line_i = 0; line_i < program->size; line_i++) {
        Line* line = &program->data[line_i];

        while (line->data[0] == ' ') {
            line->data++;
        }

        char* end_of_line = strchr(line->data, ';');
        size_t new_line_size = (size_t)(end_of_line - line->data) + 1;
        if (new_line_size > line->size) {
            fprintf(stderr, "Пропущена ;\n");
            printf("line: %lu\n", line_i + 1);
            for (int i = 0; i < line->size; i++) {
                putc(line->data[i], stdout);
            }
            putc('\n', stdout);
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

            size_t label_num = (size_t)atol(instruction_name + 1);
            if (label_num == 0) {
                fprintf(stderr, "Неверное имя метки\n");
                printf("line: %lu\n", line_i + 1);
                return;
            }

            if (label_vec->size <= label_num) {
                label_vec->capacity = (label_num + 1) * 2;
                label_vec->data = (int*)realloc(label_vec->data, label_vec->capacity * sizeof(label_vec->data[0]));

                label_vec->data[label_num] = instruction_pointer;
                label_vec->size = label_num + 1;
            }
            else {
                label_vec->data[label_num] = instruction_pointer;
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

void ProgramVecConstructor(Text* program, IntVector* program_vec, IntVector* label_vec) {
    for (size_t line_i = 0; line_i < program->size; line_i++) {
        Line line = program->data[line_i];

        char* instruction_name = line.data;

        for (size_t instruction_i = 0; instruction_i < instructions_count; instruction_i++) {
            if (strcmp(instruction_name, instructions[instruction_i].name) == 0) {
                IntVectorAdd(program_vec, (int)instruction_i);
            }
        }

        char* arg = strchr(instruction_name, '\0') + 1;
        while (arg - line.data < (ssize_t)line.size) {
            if (arg[0] == ':') {
                size_t label_num = (size_t)atol(arg + 1);
                IntVectorAdd(program_vec, label_vec->data[label_num]);
            }
            else {
                IntVectorAdd(program_vec, atoi(arg));
            }
            arg = strchr(arg, '\0') + 1;
        }
    }
}

void UploadBytecodeFile(IntVector program_vec, const char* bytecode_file_name) {
    FILE* bytecode_file = fopen(bytecode_file_name, "w");
    if (bytecode_file == NULL) {
        fprintf(stderr, "Не удалось создать файл. %s\n", strerror(errno));
        return;
    }

    fwrite(program_vec.data, sizeof(program_vec.data[0]), program_vec.size, bytecode_file);

    fclose(bytecode_file);
}

char* BytecodeFileName(char* asm_file_name) {
    size_t asm_file_name_len = strlen(asm_file_name);
    char* bytecode_file_name = (char*)calloc(asm_file_name_len + sizeof(".vovalox"), 1);
    strcpy(bytecode_file_name, asm_file_name);
    strcpy(bytecode_file_name + asm_file_name_len, ".vovalox");

    return bytecode_file_name;
}