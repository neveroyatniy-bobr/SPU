#ifndef STACK_H_
#define STACK_H_

#include <stdio.h>

enum StackError {
    STACK_OK               =  0,
    POP_ON_EMPTY_STACK     =  1,
    STACK_NULL_PTR         =  2,
    STACK_EXPANTION_ERR    =  3,
    STACK_CONTRACTION_ERR  =  4,
    STACK_INIT_ERR         =  5,
    STACK_DATA_NULL_PTR    =  6,
    STACK_OVERFLOW         =  7,
    POPED_ELEM_NULL_PTR    =  8,
    STACK_BIRD_ERROR       =  9,
    STACK_HANDLER_NULL_PTR = 10,
    STACK_HASH_ERROR       = 11
};

/// @brief Выводит сообщение об ошибке в stderr
/// @param error Код ошибки
void StackPrintError(StackError error);

typedef int stack_elem_t;

/// @brief Сруктура, хранящая стэк
struct Stack {
    size_t capacity;
    size_t size;
    stack_elem_t* data;
    size_t hash;
};

/// @brief Минимальная вместимость стэка
static const size_t STACK_MIN_CAPACITY = 16;
/// @brief Фактор увеличения вместимости стэка при переполнении
static const size_t STACK_GROW_FACTOR = 2;

/// @brief Толщина канареешной защиты
#ifndef NOBIRD
static const ssize_t BIRD_SIZE = 1;
#else
static const ssize_t BIRD_SIZE = 0;
#endif

/// @brief Значение канарейки
static const stack_elem_t BIRD_VALUE = 1890165238;

typedef void (*StackHandler)(Stack* stack, StackError error_code, const char* file, size_t line);

/// @brief Функция иницивлизации стэка
/// @param stack Стэк
/// @param capacity начальный вместимость стэка
/// @return Код ошибки
StackError StackInit(Stack* stack, size_t capacity);

/// @brief Увиличивает вместимость стэка в STACK_GROW_FACTOR раз
/// @param stack Стэк
/// @return Код ошибки
StackError StackExpantion(Stack* stack);

/// @brief Уменьшает вместимость стэка в STACK_GROW_FACTOR раз
/// @param stack Стэк
/// @return Код ошибки
StackError StackContraction(Stack* stack);

/// @brief Функция освобождения всей памяти выделенной под стэк
/// @param stack Стэк
/// @return Код ошибки
StackError StackFree(Stack* stack);

/// @brief Добавляет элемент в стэк
/// @param stack Стэк
/// @param elem Элемент
/// @return Код ошибки
StackError StackPush(Stack* stack, stack_elem_t elem);

/// @brief Удаляет последний элемент из стэка
/// @param stack Стэк
/// @param poped_elem Ссылка на переменную, в которую будет записано значение удаленного элемента
/// @return Код ошибки
StackError StackPop(Stack* stack, stack_elem_t* poped_elem);

/// @brief Проверяет стэк на ошибки
/// @param stack  Стэк
/// @return Код ошибки
StackError StackVerefy(Stack* stack);


/// @brief Выводит информацию о поломке стэка
/// @param stack Стэк
/// @param error_code Код ошибки
/// @param file Файл, в котором произошла ошибка
/// @param line Строка, в которой произошла ошибка
void StackDump(Stack* stack, StackError error_code, const char* file, size_t line);

#define StackCheck(stack)                         \
    {                                             \
        StackError err_code = StackVerefy(stack); \
        if (err_code != STACK_OK) {               \
            return err_code;                      \
        }                                         \
    }

/// @brief Стандартная функция обработки ошибок. Вызывает StackDump()
/// @param stack Стэк
/// @param error_code Код ошибки 
/// @param file Файл, в котором произошла ошибка
/// @param line Строка, в которой произошла ошибка
void StackStdHandler(Stack* stack, StackError error_code, const char* file, size_t line);

/// @brief Устанавливает хэндлер
/// @param handler Указатель на хэндлер
/// @return Код ошибки
StackError StackSetHandler(StackHandler handler);

/// @brief Устанавливает хэндлер по умолчанию
/// @return Код ошибки
StackError StackSetStdHandler();

/// @brief Функция которая вызывается при ошибке в макросе DO Smth(&stack) OR DIE(&stack);
/// @param stack Стэк
/// @param error_code Код ошибки
/// @param file Файл, в котором произошла ошибка
/// @param line Строка, в которой произошла ошибка
bool StackDie(Stack* stack, StackError error_code, const char* file, size_t line);

#define STACK_DO_OR_DIE(func, stack) { StackError err_code_ = OK; (OK == (err_code_ = func)) || StackDie(stack, err_code_, __FILE__, __LINE__); }

#endif // STACK_H_