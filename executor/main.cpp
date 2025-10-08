#include <stdio.h>

#include "processor.h"

int main() {
    Processor* processor = NULL;
    
    PROCESSOR_DO_OR_DIE(ProcessorInit(&processor), processor);

    PROCESSOR_DO_OR_DIE(Process(processor), processor);

    PROCESSOR_DO_OR_DIE(ProcessorFree(processor), processor);

    return 0;
}