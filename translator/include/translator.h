#ifndef TRANSLATOR_H_
#define TRANSLATOR_H_

#include <stdlib.h>

#include "text.h"
#include "vector.h"
#include "labels_vec.h"

static const size_t REG_COUNT = 8;
static const size_t REG_NAME_MAX_SIZE = 32;

struct Translator {
    Vector* program_vec;
    LabelsVec labels_vec;
    char reg_names[REG_COUNT][REG_NAME_MAX_SIZE];
    Text program;
};

void TranslatorInit(Translator** translator);

void TranslatorFree(Translator* translator);

void Translate(const char* asm_file_name, const char* bytecode_file_name);

void PredBytecodeConstructor(Translator* translator);

void ProgramVecConstructor(Translator* translator);

void UploadBytecodeFile(Translator* translator, const char* bytecode_file_name);

char* BytecodeFileName(char* asm_file_name);

#endif // TRANSLATOR_H_