// gcc -Werror -Wall -Wextra -Wnarrowing -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast 2_write_all.c

#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h> 
#include <stdio.h> 
#include <string.h> 
#include <stdint.h> 
#include <unistd.h> 

//собственно запись
ssize_t write_all(int fd, const void *buf, size_t count) 
{
	size_t bytes_written = 0; 
	const uint8_t *buf_adder = buf; 
	while (bytes_written < (size_t) count) 
	{
        /*
        write записывает байты count - bytes_written  из  буфера  buf_adder + bytes_written  в файл, связанный с fd
        write возвращает число записанных байтов
        */
		ssize_t res = write(fd, buf_adder + bytes_written, count - bytes_written); 
        // ошибка записи (возвращаемое значение -1 указывает на ошибку)
		if (res < 0) 
			return res; 
		bytes_written += res; 
			
	}
	return (ssize_t)bytes_written; 
}

int main(int argc, char * argv[]) 
{
	int status = 0;

    // некорректное число аргументов
	if (argc != 3) 
	{
		fprintf(stderr, "Usage: %s path text", argv[0]); 
		return 1; 
	}
	
    /*
    O_WRONLY: только запись и чтение
    O_CREATE: если файла не существовало, он будет создан
    O_TRUNC: если файл regular_file существовал, его длина уменьшится до нуля
    0644 mode: User: read & write Other: read 
    */
	int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644); 

    // беды с открытием (при успешном открытии файла open возвращает целое неотрицательное число)
	if (fd < 0) 
	{
		perror("Failed to open file for writing"); 
		return 2;
	}

    // беды с записью; write инплейсный поэтому в случае успеха все сработает
	if (write_all(fd, argv[2], strlen(argv[2])) < 0)
	{
		perror("Failed to write"); 
		status = 3;
	}

    // сбой во время закрытия (в случае успеха close возвращает 0, иначе -1)
	if (close(fd) < 0) 
	{
		perror("Failure during close"); 
		status = 4; 
	}

	return status; 
}