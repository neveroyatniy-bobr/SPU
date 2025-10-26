#include "text.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "vector.h"
#include "protected_free.h"

inline static void* VoidPtrPlus(void* ptr, const ssize_t n);

static size_t FileSize(int file);

static size_t FileSize(int file) {
    struct stat stats = {};

    if (fstat(file, &stats) != 0) {
        fprintf(stderr, "Не удалось прочитать статистику файла. %s\n", strerror(errno));
        return 0;
    }

    return (size_t)stats.st_size;
}

inline static void* VoidPtrPlus(void* ptr, const ssize_t n) {
    return (void*)((ssize_t)ptr + n);
};


void TextPrintError(TextError error) {
    switch (error) {
    case TEXT_OK:
        fprintf(stderr, "Выполнено без ошибок\n");
        break;
    case TEXT_DATA_NULL_PTR:
        fprintf(stderr, "Нулевой указатель на данные текста\n");
        break;
    case TEXT_BUFFER_NULL_PTR:
        fprintf(stderr, "Нулевой указатель на буффер текста\n");
        break;
    case TEXT_HANDLER_NULL_PTR:
        fprintf(stderr, "Нулевой указатель на хэндлер текста\n");
        break;
    case TEXT_OPEN_FILE_ERROR:
        fprintf(stderr, "Не удалось открыть файл\n");
        break;
    case TEXT_READ_FILE_ERROR:
        fprintf(stderr, "Не удалось прочитать содержимое файла\n");
        break;
    default:
        fprintf(stderr, "Непредвиденная ошибка\n");
        break;
    }
}

TextError TextInit(Text* text) {
    assert(text != NULL);

    text->buffer = NULL;

    text->data = NULL;

    text->handler = TextStdHandler;

    text->last_error_code = TEXT_OK;

    text->size = 0;

    return text->last_error_code = TEXT_OK;
}

TextError TextParse(Text* text, const char* input_file_name) {
    assert(text != NULL);
    assert(input_file_name != NULL);

    int input_file = open(input_file_name, O_RDONLY);

    if (input_file == -1) {
        return text->last_error_code = TEXT_OPEN_FILE_ERROR;
    }
    
    size_t file_size = FileSize(input_file);
    char* text_buffer = (char*)calloc(file_size + 2, sizeof(char));
    text->buffer = text_buffer;
    
    ssize_t true_file_size = read(input_file, text_buffer, file_size);

    if (true_file_size == -1) {
        return text->last_error_code = TEXT_READ_FILE_ERROR;
    }

    text_buffer[true_file_size] = '\n';
    text_buffer[true_file_size + 1] = '\0';
    
    close(input_file);
        
    Vector* text_vec = NULL;
    VECTOR_DO_OR_DIE(VectorInit(&text_vec, 16, sizeof(Line)), text_vec);
    
    char* current_line_ptr = text_buffer;
    char* next_line_ptr = text_buffer;

    while (next_line_ptr != 0 && *(next_line_ptr) != '\0') {
        current_line_ptr = next_line_ptr;
        next_line_ptr = strchr(current_line_ptr, '\n') + 1;
        
        *(next_line_ptr - 1) = '\0';

        Line current_line = { .data = current_line_ptr, .size = (size_t)(next_line_ptr - current_line_ptr - 1) };
        
        VECTOR_DO_OR_DIE(VectorPush(text_vec, &current_line), text_vec);
    }

    text->data = (Line*)text_vec->data;

    text->size = (size_t)text_vec->size;

    protected_free(text_vec);

    TextCheck(text);

    return text->last_error_code = TEXT_OK;
}

TextError TextMemoryFree(Text* text) {
    TextCheck(text);

    protected_free(text->buffer);
    
    free(VoidPtrPlus(text->data, -(ssize_t)VECTOR_BIRD_SIZE * (ssize_t)sizeof(Line)));
    text->data = NULL;

    TextCheck(text);

    return text->last_error_code = TEXT_OK;
}

TextError TextVerefy(Text* text) {
    assert(text);

    if (text->last_error_code != TEXT_OK) {
        return text->last_error_code;
    }

    if (text->data == NULL) {
        return text->last_error_code = TEXT_DATA_NULL_PTR;
    }

    if (text->buffer == NULL) {
        return text->last_error_code = TEXT_BUFFER_NULL_PTR;
    }

    if (text->handler == NULL) {
        return text->last_error_code = TEXT_HANDLER_NULL_PTR;
    }

    return text->last_error_code = TEXT_OK;
}

void TextDump(Text* text, const char* file, size_t line) {
    fprintf(stderr, "=========TEXT=DUMP========\n");

    fprintf(stderr, "ERROR in %s:%lu\n", file, line);

    TextPrintError(text->last_error_code);

    for (size_t line_i = 0; line_i < text->size; line_i++) {
        Line current_line = text->data[line_i];
        fprintf(stderr, "data[%lu].size = %lu, data[%lu].data = \"%s\"\n", line_i, current_line.size, line_i, current_line.data);
    }

    fprintf(stderr, "size = %lu\n", text->size);

    fprintf(stderr, "buffer = %p\n", text->buffer);

    fprintf(stderr, "==========================");
}

void TextStdHandler(Text* text, const char* file, size_t line) {
    TextDump(text, file, line);
}

TextError TextSetHandler(Text* text, TextHandler handler) {
    assert(text);

    if (text->handler == NULL) {
        return text->last_error_code = TEXT_HANDLER_NULL_PTR;
    }

    text->handler = handler;

    return text->last_error_code = TEXT_OK;
}

TextError TextSetStdHandler(Text* text) {
    return TextSetHandler(text, TextStdHandler);
}

bool TextDie(Text* text, const char* file, size_t line) {
    text->handler(text, file, line);
    return 0;
}