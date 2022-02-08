// gcc -Werror -Wall -Wextra -Wnarrowing -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast 3.1_copy.c

#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 4096

//та же функция, что и во 2 задаче
ssize_t write_all(int fd, const void *buf, size_t count)
{
    size_t bytes_written = 0;
    const uint8_t *buf_adder = buf;
    while (bytes_written < (size_t)count)
    {
        ssize_t res = write(fd, buf_adder + bytes_written, count - bytes_written);
        if (res < 0)
            return res;
        bytes_written += res;
    }
    return (ssize_t)bytes_written;
}

//непосредственно копирование
int my_copy(int fd_r, int fd_w)
{
    char *str_tmp;

    str_tmp = (char *)malloc(BUF_SIZE);

    //не удалось выделить память
    if (str_tmp == NULL)
    {
        perror("Failed to allocate the memory");
        return 6;
    }

    ssize_t check;

    // При успешном завершении возвращается положительное значение целое число, являющееся числом байт, помещенных в буфер
    while ((check = read(fd_r, str_tmp, BUF_SIZE)) != 0)
    {
        //ошибка чтения
        if (check < 0)
        {
            perror("Failed to read the file");
            free(str_tmp);
            return 7;
        }
        //ошибка записи
        if (write_all(fd_w, str_tmp, check) < 0)
        {
            perror("Failed to write");
            free(str_tmp);
            return 8;
        }
    }

    free(str_tmp);
    return 0;
}

int main(int argc, char *argv[])
{
    int status = 0;
    //неправильное число аргументов
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s path text", argv[0]);
        return 1;
    }

    //копирование в самого себя
    if (strcmp(argv[1], argv[2]) == 0)
    {
        perror("You can't copy the file to itself");
        return 2;
    }

    // O_RDONLY: открыт только для чтения
    int fd_r = open(argv[1], O_RDONLY);

    //ошибка чтения
    if (fd_r < 0)
    {
        perror("Failed to open file for reading");
        return 3;
    }

    //открываем на запись
    int fd_w = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    //ошибка записи
    if (fd_w < 0)
    {
        perror("Failed to open file for writing");
        status = 4;
    }

    //копирование
    int result = my_copy(fd_r, fd_w);

    // хотя бы один из файлов не получилось успешно закрыть
    if (close(fd_r) + close(fd_w) < 0)
    {
        perror("Failed while closing files")
        status = 5;
    }

    // случился косяк при копировании
    if (result > 0)
        return result;

    return status;
}