#include "instructions.h"

#include "processor.h"

ProcessorError PUSH(const int* args, Processor* processor) {
    ProcessorCheck(processor);
    
    int elem = args[0];

    StackError error_code = StackPush(&processor->stack, elem);

    if (error_code != STACK_OK) {
        return STACK_ERROR;
    }

    ProcessorCheck(processor);

    return PROCESSOR_OK;
}

ProcessorError POP(const int* /*args*/, Processor* processor) {
    ProcessorCheck(processor);

    int poped_elem = 0;
    StackError error_code = StackPop(&processor->stack, &poped_elem);

    if (error_code != STACK_OK) {
        return STACK_ERROR;
    }

    ProcessorCheck(processor);

    return PROCESSOR_OK;
}