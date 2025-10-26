#ifndef TRANSLATOR_H_
#define TRANSLATOR_H_

#include <stdlib.h>

#include "text.h"
#include "vector.h"
#include "labels.h"
#include "processor.h"

static const size_t REG_NAME_MAX_SIZE = 32;

static const char REG_START_CHAR = 'R';
static const char reg_names[REG_COUNT][REG_NAME_MAX_SIZE] = { "RAX", "RBX", "RCX", "RDX", "REX", "RFX", "RGX", "RHX" };

// FIXME как убрвть REG_NAME_MAX_SIZE

enum TranslatorError {
    TRANSLATOR_OK                  =  0,
    TRANSLATOR_HANDLER_NULL_PTR    =  1,
    DEFINES_VECTOR_ERROR           =  2,
    LABELS_VECTOR_ERROR            =  3,
    PROGRAM_VECTOR_ERROR           =  4,
    PROGRAM_TEXT_ERROR             =  5,
    TRANSLATOR_ALLOC_ERROR         =  6,
    EXPEXTED_DOT_COMMA             =  7,
    LABEL_HAS_NOT_ARGS             =  8,
    DEFINES_ARGS_COUNT_ERROR       =  9,
    INSTRUCTION_ARGS_COUNT_ERROR   = 10,
    UNDEFINED_INSTRICTION          = 11,
    UNDEFINED_REG                  = 12,
    INCORRECT_ARG                  = 13,
    CREATE_BYTECODE_FILE_ERRROR    = 14
};

struct TranslatorLog {
    const char* cpp_file_name;
    size_t cpp_line;

    bool is_asm_error;
    const char* asm_file_name;
    size_t asm_line;
};


/// @brief Выводит сообщение об ошибке в stderr
/// @param error Код ошибки
void TranslatorPrintError(TranslatorError error);

struct Translator;

typedef void (*TranslatorHandler)(Translator* translator);

struct Translator {
    Vector* program_vec;
    Vector* labels_vec;
    Vector* defines;
    char reg_names[REG_COUNT][REG_NAME_MAX_SIZE];
    Text program;
    TranslatorError last_error_code;
    TranslatorLog log;
    TranslatorHandler handler;
};

TranslatorError TranslatorInit(Translator** translator);

TranslatorError TranslatorFree(Translator* translator);

TranslatorError Translate(Translator* translator, const char* asm_file_name, const char* bytecode_file_name);

TranslatorError PredBytecodeConstructor(Translator* translator);

TranslatorError ProgramVecConstructor(Translator* translator);

TranslatorError UploadBytecodeFile(Translator* translator, const char* bytecode_file_name);

char* BytecodeFileName(char* asm_file_name);

TranslatorError BytecodeFileFree(char* bytecode_file_name);

TranslatorError LabelsArgs(Translator* translator, const char* arg);

/// @brief Проверяет транслятор на ошибки, 
/// @param translator  Транслятор
/// @return Код ошибки
TranslatorError TranslatorVerefy(Translator* translator);

/// @brief Выводит информацию о поломке транслятора
/// @param translator Транслятор
/// @param file Файл, в котором произошла ошибка
/// @param line Строка, в которой произошла ошибка
void TranslatorDump(Translator* translator);

#define TranslatorCheck(translator)                         \
    translator->last_error_code = TranslatorVerefy(translator);   \
    if (translator->last_error_code != TRANSLATOR_OK) {     \
        return translator->last_error_code;           \
    }                                            

/// @brief Стандартная функция обработки ошибок. Вызывает TranslatorDump()
/// @param translator Транслятор
/// @param file Файл, в котором произошла ошибка
/// @param line Строка, в которой произошла ошибка
void TranslatorStdHandler(Translator* translator);

/// @brief Устанавливает хэндлер
/// @param Транслятор
/// @param handler Указатель на хэндлер
/// @return Код ошибки
TranslatorError TranslatorSetHandler(Translator* translator, TranslatorHandler handler);

/// @brief Устанавливает хэндлер по умолчанию
/// @param Транслятор
/// @return Код ошибки
TranslatorError TranslatorSetStdHandler(Translator* translator);

/// @brief Функция которая вызывается при ошибке в макросе DO Smth(&translator) OR DIE(&translator);
/// @param translator Транслятор
/// @param error_code Код ошибки
/// @param file Файл, в котором произошла ошибка
/// @param line Строка, в которой произошла ошибка
bool TranslatorDie(Translator* translator);

#define TRANSLATOR_DO_OR_DIE(func, translator)                            \
    {                                                                     \
        TranslatorError translator_err_ = func;                           \
        translator->log.cpp_file_name = __FILE__;                         \
        translator->log.cpp_line = __LINE__;                              \
        (TRANSLATOR_OK == translator_err_) || TranslatorDie(translator);  \
    }

#define TRANSLATOR_TRANSLATOR_DO_OR_DIE(func, translsator)     \
    func;                                                      \
    if (translator->last_error_code != TRANSLATOR_OK) {        \
        return translator->last_error_code;                    \
    }

#endif // TRANSLATOR_H_