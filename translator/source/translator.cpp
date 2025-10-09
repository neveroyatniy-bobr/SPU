#include "translator.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "text.h"
#include "instructions.h"
#include "int_vector.h"

void Translate(const char* asm_file_name, const char* bytecode_file_name) {
    Text program = {};
    TextParse(&program, asm_file_name);

    IntVector program_vec = {};
    IntVectorInit(&program_vec, 0);

    for (size_t line_i = 0; line_i < program.size; line_i++) {
        Line line = program.data[line_i];
        char* instruction_name = strtok(line.data, " ");
        bool is_instruction = false;

        for (size_t instruction_i = 0; instruction_i < instructions_count; instruction_i++) {
            if (strcmp(instruction_name, instructions[instruction_i].name) == 0) {
                IntVectorAdd(&program_vec, (int)instruction_i);
                is_instruction = true;
            }
        }
        
        if (!is_instruction) {
            fprintf(stderr, "Несуществующая инструкция: %s\n", instruction_name);
        }

        char* arg = NULL;
        while ((arg = strtok(NULL, " ")) != NULL) {
            IntVectorAdd(&program_vec, atoi(arg));
        }
    }

    TextMemoryFree(program);

    FILE* bytecode_file = fopen(bytecode_file_name, "w");
    if (bytecode_file == NULL) {
        fprintf(stderr, "Не удалось создать файл. %s\n", strerror(errno));
        return;
    }

    fwrite(program_vec.data, sizeof(program_vec.data[0]), program_vec.size, bytecode_file);

    fclose(bytecode_file);

    IntVectorFree(&program_vec);
}

char* BytecodeFileName(char* asm_file_name) {
    size_t asm_file_name_len = strlen(asm_file_name);
    char* bytecode_file_name = (char*)calloc(asm_file_name_len + sizeof(".vovalox"), 1);
    strcpy(bytecode_file_name, asm_file_name);
    strcpy(bytecode_file_name + asm_file_name_len, ".vovalox");

    return bytecode_file_name;
}