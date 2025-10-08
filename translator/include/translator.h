#ifndef TRANSLATOR_H_
#define TRANSLATOR_H_

void Translate(const char* asm_file_name, const char* bytecode_file_name);

char* BCFileName(char* asm_file_name);

#endif // TRANSLATOR_H_