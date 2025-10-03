#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_

#include <stdio.h>

static const char* const instructions[] = {
    "PUSH",
    "POP"
};

size_t instructions_count = sizeof(instructions) / sizeof(instructions[0]);

#endif // INSTRUCTIONS_H_