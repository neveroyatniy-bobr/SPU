#ifndef MY_VECTOR_H_
#define MY_VECTOR_H_

#include <stdlib.h>

#include "text.h"

/// @brief Минимальная вместимость вектора
static const size_t MY_VECTOR_MIN_CAPACITY = 16;
/// @brief Фактор увеличения вместимости вектора при переполнении
static const size_t MY_VECTOR_GROW_FACTOR = 2;

/// @brief Структура, хранящая Вектор
struct MyVector {
    size_t capacity;
    size_t size;
    Line* data;
};

/// @brief Функция иницивлизации Вектора
/// @param vector Вектор
/// @param capacity начальный вместимость вектора
void MyVectorInit(MyVector* vector, size_t capacity);

/// @brief Увиличивает вместимость вектора в GROW_FACTOR раз
/// @param vector Вектор
void MyVectorRealloc(MyVector* vector);

/// @brief Функция освобождения всей памяти выделенной под вектор
/// @param vector Вектор
void MyVectorFree(MyVector* vector); 

/// @brief Добавляет элемент в вектор
/// @param vector Вектор
/// @param elem Элемент
void MyVectorAdd(MyVector* vector, Line elem);

#endif // MY_VECTOR_H_