#ifndef TRANSLATOR_H_
#define TRANSLATOR_H_

#include <stdlib.h>

#include "text.h"
#include "int_vector.h"
#include "my_vector.h"
#include "labels_vec.h"

void Translate(const char* asm_file_name, const char* bytecode_file_name);

void PredBytecodeConstructor(Text* program, LabelsVec* labels_vec, char reg_names[8][32]);

void ProgramVecConstructor(Text* program, IntVector* program_vec, LabelsVec* labels_vec, char reg_names[8][32]);

void UploadBytecodeFile(IntVector program_vec, const char* bytecode_file_name);

char* BytecodeFileName(char* asm_file_name);

#endif // TRANSLATOR_H_