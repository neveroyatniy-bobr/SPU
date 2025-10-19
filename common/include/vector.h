#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdio.h>

enum VectorError {
    VECTOR_OK                  =  0,
    POP_ON_EMPTY_VECTOR        =  1,
    VECTOR_NULL_PTR            =  2,
    VECTOR_EXPANTION_ERR       =  3,
    VECTOR_CONTRACTION_ERR     =  4,
    VECTOR_INIT_ERR            =  5,
    VECTOR_DATA_NULL_PTR       =  6,
    VECTOR_OVERFLOW            =  7,
    VECTOR_POPED_ELEM_NULL_PTR =  8,
    VECTOR_BIRD_ERROR          =  9,
    VECTOR_HANDLER_NULL_PTR    = 10,
    VECTOR_HASH_ERROR          = 11
};

/// @brief Выводит сообщение об ошибке в stderr
/// @param error Код ошибки
void VectorPrintError(VectorError error);

struct Vector;

typedef void (*VectorHandler)(Vector* vector, const char* file, size_t line);

/// @brief Сруктура, хранящая вектор
struct Vector {
    size_t capacity;
    size_t size;
    size_t elem_size;
    void* data;
    size_t hash;
    VectorError last_error_code;
    VectorHandler handler;
};

/// @brief Минимальная вместимость вектора
static const size_t VECTOR_MIN_CAPACITY = 16;
/// @brief Фактор увеличения вместимости вектора при переполнении
static const size_t VECTOR_GROW_FACTOR = 2;

/// @brief Толщина канареешной защиты
#ifndef NOBIRD
static const size_t VECTOR_BIRD_SIZE = 1;
#else
static const size_t VECTOR_BIRD_SIZE = 0;
#endif

/// @brief Значение канарейки
static const char BIRD_CHAR_VALUE = (char)222;

/// @brief Нулевой чар
static const size_t NULL_VALUE = 0;

/// @brief Функция иницивлизации вектора
/// @param vector Вектор
/// @param capacity Начальный вместимость вектора
/// @param elem_size Размер одного элемента
/// @return Код ошибки
VectorError VectorInit(Vector** vector, size_t elem_capacity, size_t elem_size);

/// @brief Увиличивает вместимость вектора в VECTOR_GROW_FACTOR раз
/// @param vector Вектор
/// @return Код ошибки
VectorError VectorExpantion(Vector* vector);

/// @brief Уменьшает вместимость вектора в VECTOR_GROW_FACTOR раз
/// @param vector Вектор
/// @return Код ошибки
VectorError VectorContraction(Vector* vector);

/// @brief Функция освобождения всей памяти выделенной под вектор
/// @param vector Вектор
/// @return Код ошибки
VectorError VectorFree(Vector* vector);

/// @brief Добавляет элемент в вектор
/// @param vector Вектор
/// @param elem Указатель на элемент
/// @return Код ошибки
VectorError VectorPush(Vector* vector, void* elem);

/// @brief Удаляет последний элемент из вектора
/// @param vector Вектор
/// @param poped_elem Указатель на переменную, в которую будет записано значение удаленного элемента
/// @return Код ошибки
VectorError VectorPop(Vector* vector, void* poped_elem);

/// @brief Получает значение iго элемента вектора и записывает его в dest
/// @param vector Вектор
/// @param i i
/// @param dest Указатель на переменную в которую записывается полученное значение
/// @return Код ошибки
VectorError VectorGet(Vector* vector, size_t i, void* dest);

/// @brief Записывает значение src в iй элемент вектора
/// @param vector Вектор
/// @param i i
/// @param src Указатель на переменную, значение которой будет записано 
/// @return Код ошибки
VectorError VectorSet(Vector* vector, size_t i, void* src);

/// @brief Проверяет вектор на ошибки, 
/// @param vector  Вектор
/// @return Код ошибки
VectorError VectorVerefy(Vector* vector);

/// @brief Выводит информацию о поломке вектора
/// @param vector Вектор
/// @param file Файл, в котором произошла ошибка
/// @param line Строка, в которой произошла ошибка
void VectorDump(Vector* vector, const char* file, size_t line);

#define VectorCheck(vector)                         \
    vector->last_error_code = VectorVerefy(vector); \
    if (vector->last_error_code != VECTOR_OK) {     \
        return vector->last_error_code;             \
    }                                            

/// @brief Стандартная функция обработки ошибок. Вызывает VectorDump()
/// @param vector Вектор
/// @param file Файл, в котором произошла ошибка
/// @param line Строка, в которой произошла ошибка
void VectorStdHandler(Vector* vector, const char* file, size_t line);

/// @brief Устанавливает хэндлер
/// @param Вектор
/// @param handler Указатель на хэндлер
/// @return Код ошибки
VectorError VectorSetHandler(Vector* vector, VectorHandler handler);

/// @brief Устанавливает хэндлер по умолчанию
/// @param Вектор
/// @return Код ошибки
VectorError VectorSetStdHandler(Vector* vector);

/// @brief Функция которая вызывается при ошибке в макросе DO Smth(&vector) OR DIE(&vector);
/// @param vector Вектор
/// @param error_code Код ошибки
/// @param file Файл, в котором произошла ошибка
/// @param line Строка, в которой произошла ошибка
bool VectorDie(Vector* vector, const char* file, size_t line);

#define VECTOR_DO_OR_DIE(func, vector) (VECTOR_OK == func) || VectorDie(vector, __FILE__, __LINE__);

#endif // VECTOR_H_