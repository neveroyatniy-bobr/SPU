#include "int_vector.h"

#include <assert.h>
#include <stdio.h>

void IntVectorInit(IntVector* vector, size_t capacity) {
    assert(vector != NULL);

    vector->capacity = capacity >= MIN_CAPACITY ? capacity : MIN_CAPACITY;

    vector->size = 0;

    vector->data = (int*)calloc(vector->capacity, sizeof(int));
}

void IntVectorRealloc(IntVector* vector) {
    assert(vector != NULL);

    int* data = (int*)realloc(vector->data, vector->capacity * GROW_FACTOR * sizeof(*(vector->data)));
    if (data != NULL) {
        vector->data = data;
    }

    vector->capacity *= GROW_FACTOR;
}

void IntVectorFree(IntVector* vector) {
    assert(vector != NULL);

    free(vector->data);
    
    vector->capacity = 0;
    vector->size = 0;
    vector->data = NULL;
}

void IntVectorAdd(IntVector* vector, int elem) {
    assert(vector != NULL);

    if (vector->size == vector->capacity) {
        IntVectorRealloc(vector);
    }

    vector->data[vector->size] = elem;
    vector->size++;
}

//FIXME написать обработку ошибок