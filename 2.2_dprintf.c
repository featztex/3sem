// gcc -Werror -Wall -Wextra -Wnarrowing -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast 2.1_dprintf.c

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

//тут все +- то же самое, что и в 2_write_all.c

int main(int argc, char * argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s path text", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd < 0)
    {
        perror("Failed to open file for writing");
        return 2;
    }

    // dprintf является точным аналогом функций fprintf, за исключением того, что вместо потока вывод производится в файловый дескриптор fd
    if (dprintf(fd,"%s", argv[2]) < 0)
    {
        perror("Failed to write");
        close(fd);
        return 3;
    }

    if (close(fd) < 0)
    {
        perror("Failure during close");
        return 4;
    }

    return 0;
}