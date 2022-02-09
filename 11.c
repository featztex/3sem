//gcc -Werror -Wall -Wextra -Wnarrowing -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast 11.c

#define _GNU_SOURCE // для asprintf
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <string.h>

int main(void) {

    //сюда будем сохранять результат
    const char* filename = "data.txt";

    /*
    чтение+запись
    S_IRUSR - владелец может читать
    S_IWUSR - владелец может писать
    S_IRGRP - группа-владелец может читать
    S_IROTH - все остальные могут читать
    */
    int fd = open(filename, O_RDWR | O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);

    //ошибка при открытии 
    if(fd == -1) {
        perror("Failed to open file");
        return 1;
    }

    //r+ - чтение и запись
    FILE* file = fdopen(fd, "r+");
    //в случае ошибки закрываемся
    if(!file) {
        perror("fdopen");
        close(fd);
        exit(EXIT_FAILURE);
    }

    //блокируем файл
    //LOCK_EX устанавливает эксклюзивную блокировку, только один процесс может удерживать эксклюзивную блокировку файла
    if(flock(fd, LOCK_EX) == -1) {
        perror("Failed to flock");
        fclose(file);
        return 2;
    }

    // обновляем значение в data.txt и в случае ошибки закрываемся
    int cnt;
    if(fscanf(file, "%d", &cnt) != 1) {
        cnt = 0;
    }
    cnt += 1;

    rewind(file);

    if(fprintf(file, "%d", cnt) < 0) {
        perror("Failed to write");
        fclose(file);
        return 4;
    }
    fflush(file);
    // снимаем блокировку 
    // LOCK_UN удаляет cуществующую блокировку, удерживаемую данным процессом
    if(flock(fd, LOCK_UN) == -1) {
        perror("Failed to flock: unlock");
        fclose(file);
        return 6;
    }

    fclose(file);
    return 0;
}