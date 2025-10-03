#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "text.h"
#include "instructions.h"

void Translate(const char* asm_file_name, const char* bytecode_file_name) {
    Text program = {};
    TextParse(&program, asm_file_name);

    FILE* bytecode_file = NULL;
    if ((bytecode_file = fopen(bytecode_file_name, "w")) == NULL) {
        fprintf(stderr, "Не удалось создать файл. %s\n", strerror(errno));
        return;
    }

    for (size_t line_i = 0; line_i < program.size; line_i++) {
        Line line = program.data[line_i];
        char* instruction_name = strtok(line.data, " ");
        for (int instruction_i = 0; instruction_i < instructions_count; instruction_i++) {
            if (strcmp(instruction_name, instructions[instruction_i]) == 0) {
                fprintf(bytecode_file, "%d ", instruction_i);
            }
        }

        char* arg = NULL;
        while ((arg = strtok(NULL, " ")) != NULL) {
            fprintf(bytecode_file, "%s ", arg);
        }
    }

    fclose(bytecode_file);

    MemoryFree(program);
}

char* BCFileName(char* asm_file_name) {
    int asm_file_name_len = strlen(asm_file_name);
    char* bytecode_file_name = (char*)calloc(asm_file_name_len + 9, 1);
    strcpy(bytecode_file_name, asm_file_name);
    strcpy(bytecode_file_name + asm_file_name_len, ".vovalox");

    return bytecode_file_name;
}

int main(int argc, char** argv) {
    for (int arg_i = 1; arg_i < argc; arg_i++) {
        char* asm_file_name = argv[arg_i];
        char* bytecode_file_name = BCFileName(asm_file_name);

        Translate(asm_file_name, bytecode_file_name);
        free(bytecode_file_name);
    }
}