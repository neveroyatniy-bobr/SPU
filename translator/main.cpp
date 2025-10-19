#include <stdlib.h>

#include "translator.h"
#include "vector.h"

inline static void* VoidPtrPlus(void* ptr, const ssize_t n) {
    return (void*)((ssize_t)ptr + n);
};

int main(int argc, char** argv) {for (int arg_i = 1; arg_i < argc; arg_i++) {
        char* asm_file_name = argv[arg_i];
        char* bytecode_file_name = BytecodeFileName(asm_file_name);

        Translate(asm_file_name, bytecode_file_name);

        free(bytecode_file_name); // FIXME - destructor
    }
}