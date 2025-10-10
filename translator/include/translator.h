#ifndef TRANSLATOR_H_
#define TRANSLATOR_H_

#include "text.h"
#include "int_vector.h"

void Translate(const char* asm_file_name, const char* bytecode_file_name);

void PredBytecodeConstructor(Text* program, IntVector* label_vec);

void ProgramVecConstructor(Text* program, IntVector* program_vec, IntVector* label_vec);

void UploadBytecodeFile(IntVector program_vec, const char* bytecode_file_name);

char* BytecodeFileName(char* asm_file_name);

#endif // TRANSLATOR_H_