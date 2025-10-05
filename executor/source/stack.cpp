#include "stack.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "color.h"

static Handler HANDLER = StdHandler;

inline static size_t min(size_t a, size_t b) {
    return a <= b ? a : b;
}

inline static size_t max(size_t a, size_t b) {
    return a >= b ? a : b;
}

void PrintStackError(Error error) {
    switch (error)
    {
        case OK:
            fprintf(stderr, "Выполнено без ошибок\n");
            break;
        case POP_ON_EMPTY_STACK:
            fprintf(stderr, "Попытка удалить элемент из пустого стэка\n");
            break;
        case STACK_NULL_PTR:
            fprintf(stderr, "Нулевой указатель на стэк\n");
            break;
        case STACK_EXPANTION_ERR:
            fprintf(stderr, "Ошибка увеличения памяти для стэка\n");
            break;
        case STACK_CONTRACTION_ERR:
            fprintf(stderr, "Ошибка уменьшения памяти для стэка\n");
            break;
        case STACK_INIT_ERR:
            fprintf(stderr, "Ошибка выделения памяти на стэк\n");
            break;
        case STACK_DATA_NULL_PTR:
            fprintf(stderr, "Нулевой указатель на данные стэка\n");
            break;
        case STACK_OVERFLOW:
            fprintf(stderr, "Переполнение стэка\n");
            break;
        case POPED_ELEM_NULL_PTR:
            fprintf(stderr, "Нулевой указатель на удаленный элемент\n");
            break;
        case BIRD_ERROR:
            fprintf(stderr, "Вмешательство в буффер стэка извне\n");
            break;
        case HANDLER_NULL_PTR:
            fprintf(stderr, "Попытка передать в качестве указателя на хэндлер нулевой указатель\n");
            break;
        default:
            fprintf(stderr, "Непредвиденная ошибка\n");
            break;
    }
}

Error StackInit(Stack* stack, size_t elem_capacity) {
    if (stack == NULL) {
        return STACK_NULL_PTR;
    }

    stack->capacity = max(MIN_CAPACITY, elem_capacity) + 2 * BIRD_SIZE;

    stack->size = 0;

    stack_elem_t* data = (stack_elem_t*)calloc(stack->capacity, sizeof(stack_elem_t)) + BIRD_SIZE;
    if (data == NULL) {
        return STACK_INIT_ERR;
    }
    stack->data = data;

    for (int i = 0; i < BIRD_SIZE; i++) {
        *(stack->data - BIRD_SIZE + i) = BIRD_VALUE;
        *(stack->data + stack->capacity - 2 * BIRD_SIZE + i) = BIRD_VALUE;
    }

    StackCheck(stack);

    return OK;
}

Error StackExpantion(Stack* stack) {
    StackCheck(stack);

    if (stack == NULL) {
        return STACK_NULL_PTR;
    }

    stack_elem_t* data = (stack_elem_t*)realloc(stack->data, stack->capacity * GROW_FACTOR * sizeof(*(stack->data)));
    if (data == NULL) {
        return STACK_EXPANTION_ERR;
    }
    stack->data = data;

    for (int i = 0; i < BIRD_SIZE; i++) {
        *(stack->data + stack->capacity - 2 * BIRD_SIZE + i) = 0;
    }

    stack->capacity *= GROW_FACTOR;

    for (int i = 0; i < BIRD_SIZE; i++) {
        *(stack->data + stack->capacity - 2 * BIRD_SIZE + i) = BIRD_VALUE;
    }

    StackCheck(stack);

    return OK;
}

Error StackContraction(Stack* stack) {
    StackCheck(stack);

    if (stack == NULL) {
        return STACK_NULL_PTR;
    }

    stack_elem_t* data = (stack_elem_t*)realloc(stack->data, stack->capacity / GROW_FACTOR * sizeof(*(stack->data)));
    if (data == NULL) {
        return STACK_CONTRACTION_ERR;
    }
    stack->data = data;

    for (int i = 0; i < BIRD_SIZE; i++) {
        *(stack->data + stack->capacity - 2 * BIRD_SIZE + i) = 0;
    }

    stack->capacity /= GROW_FACTOR;

    for (int i = 0; i < BIRD_SIZE; i++) {
        *(stack->data + stack->capacity - 2 * BIRD_SIZE + i) = BIRD_VALUE;
    }

    StackCheck(stack);

    return OK;
}

Error StackFree(Stack* stack) {
    StackCheck(stack);

    if (stack == NULL) {
        return STACK_NULL_PTR;
    }

    free(stack->data - BIRD_SIZE);

    stack->capacity = 0;
    stack->size = 0;
    stack->data = NULL;

    return OK;
}

Error StackAdd(Stack* stack, stack_elem_t elem) {
    StackCheck(stack);

    if (stack == NULL) {
        return STACK_NULL_PTR;
    }

    if (stack->size == (stack->capacity - 2 * BIRD_SIZE) / GROW_FACTOR) {
        StackExpantion(stack);
    }

    stack->data[stack->size] = elem;
    stack->size++;

    StackCheck(stack);

    return OK;
}

Error StackPop(Stack* stack, stack_elem_t* poped_elem) {
    StackCheck(stack);

    if (poped_elem == NULL) {
        return POPED_ELEM_NULL_PTR;
    }

    if (stack->size == 0) {
        return POP_ON_EMPTY_STACK;
    }

    *poped_elem = stack->data[stack->size - 1];

    stack->data[stack->size - 1] = 0;
    stack->size--;

    if (stack->size == (stack->capacity - 2 * BIRD_SIZE) / GROW_FACTOR - 1) {
        StackContraction(stack);
    }

    StackCheck(stack);

    return OK;
}

Error StackVerefy(Stack* stack) {
    if (stack == NULL) {
        return STACK_NULL_PTR;
    }

    if (stack->data == NULL) {
        return STACK_DATA_NULL_PTR;
    }

    if (stack->size > stack->capacity - 2 * BIRD_SIZE) {
        return STACK_OVERFLOW;
    }

    bool bird = true;
    for (int i = 0; i < BIRD_SIZE; i++) {
        bird = *(stack->data - BIRD_SIZE + i) == BIRD_VALUE ? bird : false;
        bird = *(stack->data + stack->capacity - 2 * BIRD_SIZE + i) == BIRD_VALUE ? bird : false;
    }
    if (!bird) {
        return BIRD_ERROR;
    }

    return OK;
}

void StackDump(Stack *stack, Error error_code) {
    fprintf(stderr, BRED "ERROR: ");
    PrintStackError(error_code);
    fprintf(stderr, reset);

    fprintf(stderr, BYEL "===========[STACK DUMP]==========\n" reset);
    fprintf(stderr, BYEL "|" GRN " idx " BYEL "|" GRN " value  " BYEL "|" GRN " address        " BYEL "|\n" reset);
    fprintf(stderr, BYEL "|-------------------------------|\n" reset);
    if (stack->size > 0) {
        fprintf(stderr, BYEL "|" MAG " %3lu " BYEL "|" MAG " %5d  " BYEL "|" MAG " %p " BYEL "|" GRN " <-- top\n" reset, stack->size - 1, stack->data[stack->size - 1], &stack->data[stack->size - 1]);
    }
    for (ssize_t i = (ssize_t)stack->size - 2; i >= 0; --i) {
        fprintf(stderr, BYEL "|" MAG " %3ld " BYEL "|" MAG " %5d  " BYEL "|" MAG " %p " BYEL "|\n" reset, i, stack->data[i], &stack->data[i]);
    }
    fprintf(stderr, BYEL "=================================\n" reset);
    fprintf(stderr, GRN "size " BYEL "=" MAG " %lu" BYEL "," GRN " capacity " BYEL "=" MAG " %lu" BYEL "         |\n" reset, stack->size, stack->capacity);
    fprintf(stderr, BYEL "---------------------------------\n" reset);
}

void StdHandler(Stack* stack, Error error_code) {
    StackDump(stack, error_code);
}

Error SetHandler(Handler handler) {
    if (handler == NULL) {
        return HANDLER_NULL_PTR;
    }

    HANDLER = handler;

    return OK;
}

Error SetStdHandler() {
    HANDLER = StdHandler;

    return OK;
}

bool Die(Stack* stack, Error error_code) {
    HANDLER(stack, error_code);
    return 0;
}

// FIXME Hash - защита