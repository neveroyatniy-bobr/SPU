#ifndef STACK_H_
#define STACK_H_

#include <stdlib.h>

enum Error {
    OK                    = 0,
    POP_ON_EMPTY_STACK    = 1,
    STACK_NULL_PTR        = 2,
    STACK_EXPANTION_ERR   = 3,
    STACK_CONTRACTION_ERR = 4,
    STACK_INIT_ERR        = 5,
    STACK_DATA_NULL_PTR   = 6,
    STACK_OVERFLOW        = 7,
    POPED_ELEM_NULL_PTR   = 8,
    BIRD_ERROR            = 9
};

/// @brief Выводит сообщение об ошибке в stderr
/// @param error Код ошибки
void PrintStackError(Error error);

typedef int stack_elem_t;

/// @brief Сруктура, хранящая стэк
struct Stack {
    size_t capacity;
    size_t size;
    stack_elem_t* data;
};

/// @brief Минимальная вместимость стэка
static const size_t MIN_CAPACITY = 16;
/// @brief Фактор увеличения вместимости стэка при переполнении
static const size_t GROW_FACTOR = 2;

/// @brief Толщина канареешной защиты
static const ssize_t BIRD_SIZE = 1;
/// @brief Значение канарейки
static const stack_elem_t BIRD_VALUE = 1890165238;

typedef bool (*Handler)(Stack* stack, Error error_code);

static Handler handler;

/// @brief Функция иницивлизации стэка
/// @param stack Стэк
/// @param capacity начальный вместимость стэка
/// @return Код ошибки
Error StackInit(Stack* stack, size_t capacity);

/// @brief Увиличивает вместимость стэка в GROW_FACTOR раз
/// @param stack Стэк
/// @return Код ошибки
Error StackExpantion(Stack* stack);

/// @brief Уменьшает вместимость стэка в GROW_FACTOR раз
/// @param stack Стэк
/// @return Код ошибки
Error StackContraction(Stack* stack);

/// @brief Функция освобождения всей памяти выделенной под стэк
/// @param stack Стэк
/// @return Код ошибки
Error StackFree(Stack* stack);

/// @brief Добавляет элемент в стэк
/// @param stack Стэк
/// @param elem Элемент
/// @return Код ошибки
Error StackAdd(Stack* stack, stack_elem_t elem);

/// @brief Удаляет последний элемент из стэка
/// @param stack Стэк
/// @param poped_elem Ссылка на переменную, в которую будет записано значение удаленного элемента
/// @return Код ошибки
Error StackPop(Stack* stack, stack_elem_t* poped_elem);

/// @brief Проверяет стэк на ошибки
/// @param stack  Стэк
/// @return Код ошибки
Error StackVerefy(Stack* stack);

/// @brief Выводит информацию о поломке стэка
/// @param stack Стэк
/// @param error_code Код ошибки
void StackDump(Stack* stack, Error error_code);

#define StackCheck(stack)                \
{                                        \
    Error err_code = StackVerefy(stack); \
    if (err_code != OK) {                \
        return err_code;                 \
    }                                    \
}

bool Die(Stack* stack, Error error_code);
// FIXME Сощдать глобальный указатель на хэндлер 

#define DO { Error err_code_ = OK; (OK == (err_code_ = 
#define OR )) ||
#define DIE(stack) Die(stack, err_code_); }

#endif // STACK_H_