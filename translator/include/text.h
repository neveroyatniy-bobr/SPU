#ifndef TEXT_H_ 
#define TEXT_H_

#include <stdlib.h>

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
    char* buffer_start_ptr;
};

/// @brief Парсит входной файл в массив строк. Обязательно Освободить память выделенную для указателей на элементы и сами элементы
/// @param text Указатель на структуру Text с текстом
/// @param input_file_name Название входного файла
void TextParse(Text* text, const char* input_file_name);

/// @brief Освобождает память, выделенную под текст.
/// @param text Текст
void MemoryFree(Text text);

/// @brief Вычисляет размер файла
/// @param file Указатель на файл
/// @return Размер файла в байтах
size_t FileSize(int file);

#endif // TEXT_H_