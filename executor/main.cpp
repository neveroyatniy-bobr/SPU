#include <stdio.h>

#include "processor.h"

int main() {
    Processor* processor = NULL;
    
    ProcessorInit(&processor);

    Process(processor);

    ProcessorFree(processor);

    return 0;
}