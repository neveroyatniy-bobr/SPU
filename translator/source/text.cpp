#include "text.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "my_vector.h"

static size_t FileSize(int file) {
    struct stat stats = {};

    if (fstat(file, &stats) != 0) {
        fprintf(stderr, "Не удалось прочитать статистику файла. %s\n", strerror(errno));
        return 0;
    }

    return (size_t)stats.st_size;
}

void TextParse(Text* text, const char* input_file_name) {
    assert(text != NULL);
    assert(input_file_name != NULL);

    int input_file = open(input_file_name, O_RDONLY);

    if (input_file == -1) {
        fprintf(stderr, "Не удалось открыть файл: %s. %s\n", input_file_name, strerror(errno));
        return;
    }
    
    size_t file_size = FileSize(input_file);
    char* text_buffer = (char*)calloc(file_size + 2, sizeof(char));
    text->buffer_start_ptr = text_buffer;
    
    ssize_t true_file_size = read(input_file, text_buffer, file_size);
    text_buffer[true_file_size] = '\n';
    text_buffer[true_file_size + 1] = '\0';
    
    close(input_file);
        
    MyVector text_vec = {};
    MyVectorInit(&text_vec, 16);
    
    char* current_line_ptr = text_buffer;
    char* next_line_ptr = text_buffer;

    while (next_line_ptr != 0 && *(next_line_ptr) != '\0') {
        current_line_ptr = next_line_ptr;
        next_line_ptr = strchr(current_line_ptr, '\n') + 1;
        
        *(next_line_ptr - 1) = '\0';

        Line current_line = { .data = current_line_ptr, .size = (size_t)(next_line_ptr - current_line_ptr - 1) };
        
        MyVectorAdd(&text_vec, current_line);
    }

    text->data = text_vec.data;

    text->size = text_vec.size;

}

void TextMemoryFree(Text text) {
    assert(text.data != NULL);
    assert(text.size != 0);

    free(text.buffer_start_ptr);

    free(text.data);
}