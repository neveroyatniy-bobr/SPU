#include "vector.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "color.h"

inline static size_t min(size_t a, size_t b) {
    return a <= b ? a : b;
}

inline static size_t max(size_t a, size_t b) {
    return a >= b ? a : b;
}

inline static void* VoidPtrPlus(void* ptr, const ssize_t n) {
    return (void*)((ssize_t)ptr + n);
};

void VectorPrintError(VectorError error) {
    switch (error)
    {
        case VECTOR_OK:
            fprintf(stderr, "Выполнено без ошибок\n");
            break;
        case POP_ON_EMPTY_VECTOR:
            fprintf(stderr, "Попытка удалить элемент из пустого стэка\n");
            break;
        case VECTOR_NULL_PTR:
            fprintf(stderr, "Нулевой указатель на стэк\n");
            break;
        case VECTOR_EXPANTION_ERR:
            fprintf(stderr, "Ошибка увеличения памяти для стэка\n");
            break;
        case VECTOR_CONTRACTION_ERR:
            fprintf(stderr, "Ошибка уменьшения памяти для стэка\n");
            break;
        case VECTOR_INIT_ERR:
            fprintf(stderr, "Ошибка выделения памяти на стэк\n");
            break;
        case VECTOR_DATA_NULL_PTR:
            fprintf(stderr, "Нулевой указатель на данные стэка\n");
            break;
        case VECTOR_OVERFLOW:
            fprintf(stderr, "Переполнение стэка\n");
            break;
        case VECTOR_POPED_ELEM_NULL_PTR:
            fprintf(stderr, "Нулевой указатель на удаленный элемент\n");
            break;
        case VECTOR_BIRD_ERROR:
            fprintf(stderr, "Канарейка: Вмешательство в буффер стэка извне\n");
            break;
        case VECTOR_HANDLER_NULL_PTR:
            fprintf(stderr, "Попытка передать в качестве указателя на хэндлер нулевой указатель\n");
            break;
        case VECTOR_HASH_ERROR:
            fprintf(stderr, "Хэш: Вмешательство в буффер стэка извне\n");
            break;
        default:
            fprintf(stderr, "Непредвиденная ошибка\n");
            break;
    }
}

#ifdef HASH_PROTECTED
static size_t VectorHash(Vector* vector) {
    size_t hash = 0;
    hash += vector->capacity;
    hash += vector->size;
    for (ssize_t i = -VECTOR_BIRD_SIZE * vector->elem_size; i < vector->size + VECTOR_BIRD_SIZE * vector->elem_size; i++) {
        hash += (size_t)*(char*)VoidPtrPlus(vector->data, i);
    }
    return hash;
}
#else
static size_t VectorHash(Vector* /* vector */) {
    return 0;
}
#endif

VectorError VectorVerefy(Vector* vector) {
    if (vector == NULL) {
        return vector->last_error_code = VECTOR_NULL_PTR;
    }

    if (vector->last_error_code != VECTOR_OK) {
        return vector->last_error_code;
    }

    if (vector->data == NULL) {
        return vector->last_error_code = VECTOR_DATA_NULL_PTR;
    }

    if (vector->size > vector->capacity - 2 * VECTOR_BIRD_SIZE) {
        return vector->last_error_code = VECTOR_OVERFLOW;
    }

    bool bird = true;

    for (ssize_t i = 0; i < (ssize_t)VECTOR_BIRD_SIZE * (ssize_t)vector->elem_size; i++) {
        bird = memcmp(VoidPtrPlus(vector->data, - (ssize_t)VECTOR_BIRD_SIZE * (ssize_t)vector->elem_size + i), &BIRD_CHAR_VALUE, 1) == 0 ? bird : false;
        bird = memcmp(VoidPtrPlus(vector->data, ((ssize_t)vector->capacity - 2 * (ssize_t)VECTOR_BIRD_SIZE) * (ssize_t)vector->elem_size + i), &BIRD_CHAR_VALUE, 1) == 0 ? bird : false;
    }

    for (ssize_t i = 0; i < (ssize_t)VECTOR_BIRD_SIZE * (ssize_t)vector->elem_size; i++) {
        memcpy(VoidPtrPlus(vector->data, - (ssize_t)VECTOR_BIRD_SIZE * (ssize_t)vector->elem_size + i), &BIRD_CHAR_VALUE, 1);
        memcpy(VoidPtrPlus(vector->data, ((ssize_t)vector->capacity - 2 * (ssize_t)VECTOR_BIRD_SIZE) * (ssize_t)vector->elem_size + i), &BIRD_CHAR_VALUE, 1);
    }

    if (!bird) {
        return vector->last_error_code = VECTOR_BIRD_ERROR;
    }

    if (vector->hash != VectorHash(vector)) {
        return vector->last_error_code = VECTOR_HASH_ERROR;
    }

    return vector->last_error_code = VECTOR_OK;
}

void VectorDump(Vector *vector, const char* file, size_t line) {
    fprintf(stderr, BRED "ERROR in %s:%lu: ", file, line);
    VectorPrintError(vector->last_error_code);
    fprintf(stderr, reset);

    fprintf(stderr, BYEL "==========[VECTOR DUMP]==========\n" reset);
    fprintf(stderr, BYEL "|" GRN " idx " BYEL "|" GRN " value  " BYEL "|" GRN " address        " BYEL "|\n" reset);
    fprintf(stderr, BYEL "|-------------------------------|\n" reset);
    if (vector->size > 0) {
        fprintf(stderr, BYEL "|" MAG " %3ld " BYEL "|" MAG " %5ld  " BYEL "|" MAG " %p " BYEL "|" GRN " <-- end\n" reset, (ssize_t)vector->size - 1, *(long int*)VoidPtrPlus(vector->data, (ssize_t)vector->size - 1), VoidPtrPlus(vector->data, (ssize_t)vector->size - 1));
    }
    for (ssize_t i = (ssize_t)vector->size - 2; i >= 0; --i) {
        fprintf(stderr, BYEL "|" MAG " %3ld " BYEL "|" MAG " %5ld  " BYEL "|" MAG " %p " BYEL "|\n" reset, i, *(long int*)VoidPtrPlus(vector->data, i), VoidPtrPlus(vector->data, i));
    }
    fprintf(stderr, BYEL "=================================\n" reset);
    fprintf(stderr, GRN "size " BYEL "=" MAG " %lu" BYEL "," GRN " elem_size " BYEL "=" MAG " %lu" BYEL "," GRN " capacity " BYEL "=" MAG " %lu" "\n" reset, vector->size, vector->elem_size, vector->capacity);
    fprintf(stderr, BYEL "---------------------------------\n");
    for (ssize_t i = -(ssize_t)VECTOR_BIRD_SIZE * (ssize_t)vector->elem_size; i < ((ssize_t)vector->capacity - (ssize_t)VECTOR_BIRD_SIZE) * (ssize_t)vector->elem_size; i++) {
        fprintf(stderr, "data[%ld] = %u\n", i, (int)*(unsigned char*)VoidPtrPlus(vector->data, i));
    }
    fprintf(stderr, reset);
}

void VectorStdHandler(Vector* vector, const char* file, size_t line) {
    VectorDump(vector, file, line);
    abort();
}

VectorError VectorSetHandler(Vector* vector, VectorHandler handler) {
    if (vector->handler == NULL) {
        return vector->last_error_code = VECTOR_HANDLER_NULL_PTR;
    }

    vector->handler = handler;

    return vector->last_error_code = VECTOR_OK;
}

VectorError VectorSetStdHandler(Vector* vector) {
    vector->handler = VectorStdHandler;
    return vector->last_error_code = VECTOR_OK;
}

bool VectorDie(Vector* vector, const char* file, size_t line) {
    vector->handler(vector, file, line);
    return 0;
}

VectorError VectorInit(Vector** vector, size_t capacity, size_t elem_size) {
    *vector = (Vector*)calloc(1, sizeof(Vector));

    (*vector)->capacity = max(VECTOR_MIN_CAPACITY, capacity) + 2 * VECTOR_BIRD_SIZE;

    (*vector)->size = 0;

    (*vector)->elem_size = elem_size;

    void* data = VoidPtrPlus(calloc((*vector)->capacity, (*vector)->elem_size), (ssize_t)VECTOR_BIRD_SIZE * (ssize_t)(*vector)->elem_size);
    if (data == NULL) {
        return (*vector)->last_error_code = VECTOR_INIT_ERR;
    }
    (*vector)->data = data;

    for (ssize_t i = 0; i < (ssize_t)VECTOR_BIRD_SIZE * (ssize_t)(*vector)->elem_size; i++) {
        memcpy(VoidPtrPlus((*vector)->data, - (ssize_t)VECTOR_BIRD_SIZE * (ssize_t)(*vector)->elem_size + i), &BIRD_CHAR_VALUE, 1);
        memcpy(VoidPtrPlus((*vector)->data, ((ssize_t)(*vector)->capacity - 2 * (ssize_t)VECTOR_BIRD_SIZE) * (ssize_t)(*vector)->elem_size + i), &BIRD_CHAR_VALUE, 1);
    }

    (*vector)->hash = VectorHash(*vector);

    (*vector)->last_error_code = VECTOR_OK;

    (*vector)->handler = VectorStdHandler;

    VectorCheck((*vector));

    return (*vector)->last_error_code = VECTOR_OK;
}

VectorError VectorExpantion(Vector* vector) {
    VectorCheck(vector);

    if (vector == NULL) {
        return vector->last_error_code = VECTOR_NULL_PTR;
    }

    void* data = VoidPtrPlus(realloc(VoidPtrPlus(vector->data, - (ssize_t)VECTOR_BIRD_SIZE * (ssize_t)vector->elem_size), vector->capacity * VECTOR_GROW_FACTOR * vector->elem_size), (ssize_t)VECTOR_BIRD_SIZE * (ssize_t)vector->elem_size);
    if (data == NULL) {
        return vector->last_error_code = VECTOR_EXPANTION_ERR;
    }
    vector->data = data;

    for (ssize_t i = 0; i < (ssize_t)VECTOR_BIRD_SIZE * (ssize_t)vector->elem_size; i++) {
        memcpy(VoidPtrPlus(vector->data, ((ssize_t)vector->capacity - 2 * (ssize_t)VECTOR_BIRD_SIZE) * (ssize_t)vector->elem_size + i), &NULL_VALUE, 1);
    }

    vector->capacity = (vector->capacity - 2 * VECTOR_BIRD_SIZE) * VECTOR_GROW_FACTOR + 2 * VECTOR_BIRD_SIZE;

    for (ssize_t i = 0; i < (ssize_t)VECTOR_BIRD_SIZE * (ssize_t)vector->elem_size; i++) {
        memcpy(VoidPtrPlus(vector->data, ((ssize_t)vector->capacity - 2 * (ssize_t)VECTOR_BIRD_SIZE) * (ssize_t)vector->elem_size + i), &BIRD_CHAR_VALUE, 1);
    }

    vector->hash = VectorHash(vector);

    VectorCheck(vector);

    return vector->last_error_code = VECTOR_OK;
}

VectorError VectorContraction(Vector* vector) {
    VectorCheck(vector);

    if (vector == NULL) {
        return vector->last_error_code = VECTOR_NULL_PTR;
    }

    void* data = VoidPtrPlus(realloc(VoidPtrPlus(vector->data, - (ssize_t)VECTOR_BIRD_SIZE * (ssize_t)vector->elem_size), vector->capacity / VECTOR_GROW_FACTOR * vector->elem_size), (ssize_t)VECTOR_BIRD_SIZE * (ssize_t)vector->elem_size);
    if (data == NULL) {
        return vector->last_error_code = VECTOR_CONTRACTION_ERR;
    }
    vector->data = data;

    for (ssize_t i = 0; i < (ssize_t)VECTOR_BIRD_SIZE * (ssize_t)vector->elem_size; i++) {
        memcpy(VoidPtrPlus(vector->data, ((ssize_t)vector->capacity - 2 * (ssize_t)VECTOR_BIRD_SIZE) * (ssize_t)vector->elem_size + i), &NULL_VALUE, 1);
    }

    vector->capacity /= VECTOR_GROW_FACTOR;

    for (ssize_t i = 0; i < (ssize_t)VECTOR_BIRD_SIZE * (ssize_t)vector->elem_size; i++) {
        memcpy(VoidPtrPlus(vector->data, ((ssize_t)vector->capacity - 2 * (ssize_t)VECTOR_BIRD_SIZE) * (ssize_t)vector->elem_size + i), &BIRD_CHAR_VALUE, 1);
    }

    vector->hash = VectorHash(vector);

    VectorCheck(vector);

    return vector->last_error_code = VECTOR_OK;
}

VectorError VectorFree(Vector* vector) {
    VectorCheck(vector);

    if (vector == NULL) {
        return vector->last_error_code = VECTOR_NULL_PTR;
    }

    free(VoidPtrPlus(vector->data, - (ssize_t)VECTOR_BIRD_SIZE * (ssize_t)vector->elem_size));

    vector->capacity = 0;
    vector->size = 0;
    vector->data = NULL;

    free(vector);

    return VECTOR_OK;
}

VectorError VectorPush(Vector* vector, void* elem) {
    VectorCheck(vector);

    if (vector == NULL) {
        return vector->last_error_code = VECTOR_NULL_PTR;
    }

    if (vector->size == vector->capacity - 2 * VECTOR_BIRD_SIZE) {
        VectorExpantion(vector);
    }

    memcpy(VoidPtrPlus(vector->data, (ssize_t)vector->size * (ssize_t)vector->elem_size),  elem, vector->elem_size);
    vector->size++;

    vector->hash = VectorHash(vector);

    VectorCheck(vector);

    return vector->last_error_code = VECTOR_OK;
}

VectorError VectorPop(Vector* vector, void* poped_elem) {
    VectorCheck(vector);

    if (poped_elem == NULL) {
        return vector->last_error_code = VECTOR_POPED_ELEM_NULL_PTR;
    }

    if (vector->size == 0) {
        return vector->last_error_code = POP_ON_EMPTY_VECTOR;
    }

    memcpy(poped_elem, VoidPtrPlus(vector->data, ((ssize_t)vector->size - 1) * (ssize_t)vector->elem_size), vector->elem_size);

    memcpy(VoidPtrPlus(vector->data, (ssize_t)vector->size - 1), &NULL_VALUE, vector->elem_size);

    vector->size--;

    if (vector->size == (vector->capacity - 2 * VECTOR_BIRD_SIZE) / VECTOR_GROW_FACTOR - 1) {
        VectorContraction(vector);
    }

    vector->hash = VectorHash(vector);

    VectorCheck(vector);

    return vector->last_error_code = VECTOR_OK;
}

VectorError VectorGet(Vector* vector, size_t i, void* dest){
    assert(dest);

    VectorCheck(vector);

    if (vector->size == 0) {
        return vector->last_error_code = POP_ON_EMPTY_VECTOR;
    }

    memcpy(dest, VoidPtrPlus(vector->data, (ssize_t)i * (ssize_t)vector->elem_size), vector->elem_size);

    vector->hash = VectorHash(vector);

    VectorCheck(vector);

    return vector->last_error_code = VECTOR_OK;
}

VectorError VectorSet(Vector* vector, size_t i, void* src) {
    assert(src);

    VectorCheck(vector);

    if (vector->size == 0) {
        return vector->last_error_code = POP_ON_EMPTY_VECTOR;
    }

    memcpy(VoidPtrPlus(vector->data, (ssize_t)i * (ssize_t)vector->elem_size), src, vector->elem_size);

    vector->hash = VectorHash(vector);

    VectorCheck(vector);

    return vector->last_error_code = VECTOR_OK;
}