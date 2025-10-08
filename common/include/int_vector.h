#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdlib.h>

/// @brief Минимальная вместимость вектора
static const size_t MIN_CAPACITY = 16;
/// @brief Фактор увеличения вместимости вектора при переполнении
static const size_t GROW_FACTOR = 2;

/// @brief Структура, хранящая Вектор
struct IntVector {
    size_t capacity;
    size_t size;
    int* data;
};

/// @brief Функция иницивлизации Вектора
/// @param vector Вектор
/// @param capsacity начальный вместимость вектора
void IntVectorInit(IntVector* vector, size_t capacity);

/// @brief Увиличивает вместимость вектора в GROW_FACTOR раз
/// @param vector Вектор
void IntVectorRealloc(IntVector* vector);

/// @brief Функция освобождения всей памяти выделенной под вектор
/// @param vector Вектор
void IntVectorFree(IntVector* vector); 

/// @brief Добавляет элемент в вектор
/// @param vector Вектор
/// @param elem Элемент
void IntVectorAdd(IntVector* vector, int elem);

#endif // VECTOR_H_