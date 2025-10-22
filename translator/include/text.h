#ifndef TEXT_H_ 
#define TEXT_H_

#include <stdlib.h>

enum TextError {
    TEXT_OK                  =  0,
    TEXT_DATA_NULL_PTR       =  1,
    TEXT_BUFFER_NULL_PTR     =  2,
    TEXT_HANDLER_NULL_PTR    =  3
};

/// @brief Выводит сообщение об ошибке в stderr
/// @param error Код ошибки
void TextPrintError(TextError error);

struct Text;

typedef void (*TextHandler)(Text* text, const char* file, size_t line);

/// @brief Хранит линию из текста
struct Line
{
    char* data;
    size_t size;
};

/// @brief Хранит текст
struct Text {
    Line* data;
    size_t size;
    char* buffer;
    TextError last_error_code;
    TextHandler handler;
};

/// @brief Инициализирует текст
/// @param text Текст
/// @return Код ошибки
TextError TextInit(Text* text);

/// @brief Парсит входной файл в массив строк. Обязательно Освободить память выделенную для указателей на элементы и сами элементы
/// @param text Указатель на структуру Text с текстом
/// @param input_file_name Название входного файла
/// @return Код ошибки
TextError TextParse(Text* text, const char* input_file_name);

/// @brief Освобождает память, выделенную под текст.
/// @param text Текст
/// @return Код ошибки
TextError TextMemoryFree(Text* text);

/// @brief Проверяет текст на ошибки, 
/// @param text  Текст
/// @return Код ошибки
TextError TextVerefy(Text* text);

/// @brief Выводит информацию о поломке текста
/// @param text Текст
/// @param file Файл, в котором произошла ошибка
/// @param line Строка, в которой произошла ошибка
void TextDump(Text* text, const char* file, size_t line);

#define TextCheck(text)                         \
    text->last_error_code = TextVerefy(text);   \
    if (text->last_error_code != TEXT_OK) {     \
        return text->last_error_code;           \
    }                                            

/// @brief Стандартная функция обработки ошибок. Вызывает TextDump()
/// @param text Текст
/// @param file Файл, в котором произошла ошибка
/// @param line Строка, в которой произошла ошибка
void TextStdHandler(Text* text, const char* file, size_t line);

/// @brief Устанавливает хэндлер
/// @param Текст
/// @param handler Указатель на хэндлер
/// @return Код ошибки
TextError TextSetHandler(Text* text, TextHandler handler);

/// @brief Устанавливает хэндлер по умолчанию
/// @param Текст
/// @return Код ошибки
TextError TextSetStdHandler(Text* text);

/// @brief Функция которая вызывается при ошибке в макросе DO Smth(&text) OR DIE(&text);
/// @param text Текст
/// @param error_code Код ошибки
/// @param file Файл, в котором произошла ошибка
/// @param line Строка, в которой произошла ошибка
bool TextDie(Text* text, const char* file, size_t line);

#define TEXT_DO_OR_DIE(func, text) (TEXT_OK == func) || TextDie(text, __FILE__, __LINE__);

#endif // TEXT_H_