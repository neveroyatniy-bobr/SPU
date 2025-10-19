#include <stdio.h>

#include "processor.h"

int main(int argc, char** argv) {
    for (int file_i = 1; file_i < argc; file_i++) {
        Processor* processor = NULL;
        
        PROCESSOR_DO_OR_DIE(ProcessorInit(&processor, argv[file_i]), processor);

        PROCESSOR_DO_OR_DIE(Process(processor), processor);

        PROCESSOR_DO_OR_DIE(ProcessorFree(processor), processor);
    }

    return 0;
}