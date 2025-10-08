#include "stack.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "color.h"

static StackHandler HANDLER = StackStdHandler;

inline static size_t min(size_t a, size_t b) {
    return a <= b ? a : b;
}

inline static size_t max(size_t a, size_t b) {
    return a >= b ? a : b;
}

void StackPrintError(StackError error) {
    switch (error)
    {
        case STACK_OK:
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
        case STACK_BIRD_ERROR:
            fprintf(stderr, "Канарейка: Вмешательство в буффер стэка извне\n");
            break;
        case STACK_HANDLER_NULL_PTR:
            fprintf(stderr, "Попытка передать в качестве указателя на хэндлер нулевой указатель\n");
            break;
        case STACK_HASH_ERROR:
            fprintf(stderr, "Хэш: Вмешательство в буффер стэка извне\n");
            break;
        default:
            fprintf(stderr, "Непредвиденная ошибка\n");
            break;
    }
}

#ifdef HASH_PROTECTED
static size_t StackHash(Stack* stack) {
    size_t hash = 0;
    hash += stack->capacity;
    hash += stack->size;
    for (ssize_t i = -BIRD_SIZE; i < (ssize_t)stack->size + BIRD_SIZE; i++) {
        hash += (size_t)stack->data[i];
    }
    return hash;
}
#else
static size_t StackHash(Stack* /* stack */) {
    return 0;
}
#endif

StackError StackVerefy(Stack* stack) {
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
        return STACK_BIRD_ERROR;
    }

    if (stack->hash != StackHash(stack)) {
        return STACK_HASH_ERROR;
    }

    return STACK_OK;
}

void StackDump(Stack *stack, StackError error_code, const char* file, size_t line) {
    fprintf(stderr, BRED "ERROR in %s:%lu: ", file, line);
    StackPrintError(error_code);
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

void StackStdHandler(Stack* stack, StackError error_code, const char* file, size_t line) {
    StackDump(stack, error_code, file, line);
}

StackError StackSetHandler(StackHandler handler) {
    if (handler == NULL) {
        return STACK_HANDLER_NULL_PTR;
    }

    HANDLER = handler;

    return STACK_OK;
}

StackError StackSetStdHandler() {
    HANDLER = StackStdHandler;
    return STACK_OK;
}

bool StackDie(Stack* stack, StackError error_code, const char* file, size_t line) {
    HANDLER(stack, error_code, file, line);
    return 0;
}

StackError StackInit(Stack* stack, size_t elem_capacity) {
    if (stack == NULL) {
        return STACK_NULL_PTR;
    }

    stack->capacity = max(STACK_MIN_CAPACITY, elem_capacity) + 2 * BIRD_SIZE;

    stack->size = 0;

    stack_elem_t* data = (stack_elem_t*)calloc(stack->capacity, sizeof(stack_elem_t)) + BIRD_SIZE;
    if (data == NULL) {
        return STACK_INIT_ERR;
    }
    stack->data = data;

    for (ssize_t i = 0; i < BIRD_SIZE; i++) {
        stack->data[- BIRD_SIZE + i] = BIRD_VALUE;
        stack->data[(ssize_t)stack->capacity - 2 * BIRD_SIZE + i] = BIRD_VALUE;
    }

    stack->hash = StackHash(stack);

    StackCheck(stack);

    return STACK_OK;
}

StackError StackExpantion(Stack* stack) {
    StackCheck(stack);

    if (stack == NULL) {
        return STACK_NULL_PTR;
    }

    stack_elem_t* data = (stack_elem_t*)realloc(stack->data, stack->capacity * STACK_GROW_FACTOR * sizeof(*(stack->data)));
    if (data == NULL) {
        return STACK_EXPANTION_ERR;
    }
    stack->data = data;

    for (int i = 0; i < BIRD_SIZE; i++) {
        *(stack->data + stack->capacity - 2 * BIRD_SIZE + i) = 0;
    }

    stack->capacity *= STACK_GROW_FACTOR;

    for (int i = 0; i < BIRD_SIZE; i++) {
        *(stack->data + stack->capacity - 2 * BIRD_SIZE + i) = BIRD_VALUE;
    }

    stack->hash = StackHash(stack);

    StackCheck(stack);

    return STACK_OK;
}

StackError StackContraction(Stack* stack) {
    StackCheck(stack);

    if (stack == NULL) {
        return STACK_NULL_PTR;
    }

    stack_elem_t* data = (stack_elem_t*)realloc(stack->data, stack->capacity / STACK_GROW_FACTOR * sizeof(*(stack->data)));
    if (data == NULL) {
        return STACK_CONTRACTION_ERR;
    }
    stack->data = data;

    for (int i = 0; i < BIRD_SIZE; i++) {
        *(stack->data + stack->capacity - 2 * BIRD_SIZE + i) = 0;
    }

    stack->capacity /= STACK_GROW_FACTOR;

    for (int i = 0; i < BIRD_SIZE; i++) {
        *(stack->data + stack->capacity - 2 * BIRD_SIZE + i) = BIRD_VALUE;
    }

    stack->hash = StackHash(stack);

    StackCheck(stack);

    return STACK_OK;
}

StackError StackFree(Stack* stack) {
    StackCheck(stack);

    if (stack == NULL) {
        return STACK_NULL_PTR;
    }

    free(stack->data - BIRD_SIZE);

    stack->capacity = 0;
    stack->size = 0;
    stack->data = NULL;

    return STACK_OK;
}

StackError StackPush(Stack* stack, stack_elem_t elem) {
    StackCheck(stack);

    if (stack == NULL) {
        return STACK_NULL_PTR;
    }

    if (stack->size == (stack->capacity - 2 * BIRD_SIZE) / STACK_GROW_FACTOR) {
        StackExpantion(stack);
    }

    stack->data[stack->size] = elem;
    stack->size++;

    stack->hash = StackHash(stack);

    StackCheck(stack);

    return STACK_OK;
}

StackError StackPop(Stack* stack, stack_elem_t* poped_elem) {
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

    if (stack->size == (stack->capacity - 2 * BIRD_SIZE) / STACK_GROW_FACTOR - 1) {
        StackContraction(stack);
    }

    stack->hash = StackHash(stack);

    StackCheck(stack);

    return STACK_OK;
}