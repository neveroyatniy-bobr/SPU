#include <stdlib.h>

#include "translator.h"
#include "vector.h"
#include "protected_free.h"

int main(int argc, char** argv) {
    for (int arg_i = 1; arg_i < argc; arg_i++) {
        char* asm_file_name = argv[arg_i];
        char* bytecode_file_name = BytecodeFileName(asm_file_name);

        Translator* translator = NULL;

        TRANSLATOR_DO_OR_DIE(TranslatorInit(&translator), translator);

        TRANSLATOR_DO_OR_DIE(Translate(translator, asm_file_name, bytecode_file_name), translator);

        TRANSLATOR_DO_OR_DIE(BytecodeFileFree(bytecode_file_name), translator);

        TranslatorFree(translator), translator;
    }
}