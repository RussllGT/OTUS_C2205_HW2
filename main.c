#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <malloc.h>
#include <errno.h>
#include <sys\stat.h>
#include "decode.h"
#include "cp1251_unicode.h"
#include "koi8_unicode.h"
#include "iso88595_unicode.h"

/// <summary>
/// Сравнивает строки
/// </summary>
/// <param name="first">Первая строка</param>
/// <param name="second">Вторая строка</param>
/// <returns>Возвращает true, если переданные строки равны (без учета нуль-терминатора), в противном случае false</returns>
bool isCharEqual(const char* first, const char* second)
{
    bool result = true;
    while (*first && *second)
    {
        if (*(first++) != *(second++))
        {
            result = false;
            break;
        }
    }
    return result;
}

int main(int argc, char* argv[])
{    
    //Если число аргументов не равно 4 закрываем программу с сообщением
    if (argc != 4)
    {
        printf("Error: Arguments are incorrect\n");
        return 0;
    }

    //Чтение файла из агрумента командной строки. В случае ошибки - выводим сообщение и закрываем программу
    FILE* file = fopen(argv[1], "rb");
    if (!file)
    {
        char* errorbuf = strerror(errno);
        fprintf(stderr, "Error: %s\n", errorbuf);
        errno = 0;
        return 0;
    }

    //Получаем размер файла через структуру данных о файле
    struct stat filestat;
    stat(argv[1], &filestat);
    size_t len = filestat.st_size;

    //Проверяем, что переданная сущность является файлом
    /*if (S_ISREG(filestat.st_mode) == 0)
    {
        printf("Error: Target path is not a file\n");
        return 0;
    }*/

    // Выбираем таблицу в зависимости от переданного аргумента кодировки
    symbol_t* table;
    if (isCharEqual(argv[2], "cp1251"))
    {
        table = get_cp1251_to_unicode_table();
    }
    else if (isCharEqual(argv[2], "iso-8859-5"))
    {
        table = get_iso88595_to_unicode_table();
    }
    else if (isCharEqual(argv[2], "koi8"))
    {
        table = get_koi8_to_unicode_table();
    }
    else
    {
        printf("Error: Encoding is incorrect\n");
        return 1;
    }

    //Читаем файл в буфер и закрываем файл
    uint8_t* buff = (uint8_t*)malloc(sizeof(uint8_t) * len);
    uint8_t* endbuff = buff + len;
    for (int i = 0; i < len; i++) *(buff + i) = getc(file);
    fclose(file);

    //Выделяем память и записываем в неё перекодированные данные
    uint8_t* str = (uint8_t*)malloc(sizeof(uint8_t) * len * 2);
    uint8_t* endstr = write_output(str, buff, endbuff, table);

    //Освобождаем буфер исходных данных
    free(buff);

    //Создаем файл из переданного аргумента и записываем в него данные из перекодированного буфера
    FILE* res = fopen(argv[3], "wb");
    uint8_t* write = str;
    while (write != endstr)
    {
        putc(*(write++), res);
    }

    //Закрываем файл и освобождаем перекодированный буфер
    fclose(res);
    free(str);

    return 0;
}