#include <stdio.h>

#include "stack.h"

int main() {
    Stack stack = {};

    DO StackInit(&stack, 1) OR DIE(&stack);

    DO StackAdd(&stack, 1) OR DIE(&stack);
    DO StackAdd(&stack, 2) OR DIE(&stack);
    DO StackAdd(&stack, 2) OR DIE(&stack);

    printf("added\n");

    stack.data[0] = 11;

    DO StackVerefy(&stack) OR DIE(&stack);

    int poped_elem = 0;

    DO StackPop(&stack, &poped_elem) OR DIE(&stack);
    printf("%d\n", poped_elem);
    DO StackPop(&stack, &poped_elem) OR DIE(&stack);
    printf("%d\n", poped_elem);
    DO StackPop(&stack, &poped_elem) OR DIE(&stack);
    printf("%d\n", poped_elem);
    DO StackPop(&stack, &poped_elem) OR DIE(&stack);
    printf("%d\n", poped_elem);

    DO StackVerefy(&stack) OR DIE(&stack);

    printf("poped\n");

    DO StackFree(&stack) OR DIE(&stack);

    return 0;
}