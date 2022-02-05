#define _FILE_OFFSET_BITS 64 //у нас 64-битная система


#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


//функция для определения типа 
const char* device_type(const mode_t mode) {
    switch (mode & S_IFMT) {
        case S_IFBLK:   return "block device"; // блочное устройство (например, жесткий диск или компакт-диск)
        case S_IFCHR:   return "character device"; // символьное устройство (посимвольный обмен, пример - модем)
        case S_IFDIR:   return "directory"; // каталог (папка)
        case S_IFIFO:   return "FIFO/pipe"; //FIFO-файл (именованный канал)
        case S_IFLNK:   return "symbolic link"; //символическая ссылка (похожа на ярлык)
        case S_IFREG:   return "regular file"; //обычный файл
        case S_IFSOCK:  return "socket"; // сокет - название программного интерфейса для обеспечения обмена данными между процессами
    }
    return "unknown";
}


int main(int argc, char* argv[]) {

    // ошибка, если подано неправильное число аргументов
    if(argc != 2) {
        fprintf(stderr, "Incorrect argc: %d\n", argc); //stderr - выходной (нет блин будний) поток для вывода ошибок
        exit(EXIT_FAILURE); // используется для возвращения ОС кода о неудачном завершении программы
    }

    struct stat sb;

    if(lstat(argv[1], &sb) == -1) { // кок
        perror("lstat"); // вывод сообщения об ошибке в поток сообщений об ошибке
        exit(EXIT_FAILURE); 
    }

    printf("File type:              %s\n", device_type(sb.st_mode)); // тип файла
    printf("INode:                  %ld\n", (long)sb.st_ino); // номер индексного дескриптора, inode - структура, содержащая метаданные файла
    printf("Access mode:            %lo (octal)\n", (unsigned long)sb.st_mode); // это поле  содержит биты  прав доступа  к файлу,  тип файла и специальные биты
    printf("Number of links:        %ld\n", (long)sb.st_nlink); // число ссылающихся  на заданный  файл записей в директориях
    printf("Owner:                  UID=%ld\tGID=%ld\n", (long)sb.st_uid, (long)sb.st_gid); // st_uid - пользовательский идентификатор владельца файла, st_gid - идентификатор группы заданного файла.
    printf("Preferred block size:   %ld byte\n", (long)sb.st_blksize); // задает "предпочтительный" размер блока для эффективного ввода/вывода в файловой системе
    printf("File size:              %lld byte\n", (long long)sb.st_size); // размер файла
    printf("Allocated blocks:       %lld\n", (long long)sb.st_blocks); // задает размер файла в 512-байтных блоках


    const size_t time_str_size = sizeof("Day Mon dd hh:mm:ss yyyy\n");

    struct tm curr_time;
    char buf[time_str_size];

    const char fmt[] = "%a %b %d %H:%M:%S %Y";
    
    if( !strftime(buf, sizeof(buf), fmt, gmtime_r(&sb.st_ctime, &curr_time)) ) {
        exit(EXIT_FAILURE);
    }

    //  время последнего изменения атрибутов файла (данных, которые хранятся в inode-области), например, создание жестких ссылок на файл
    printf("C_TIME                  %s\n", buf);
    if( !strftime(buf, sizeof(buf), fmt, gmtime_r(&sb.st_atime, &curr_time)) ) {
        exit(EXIT_FAILURE);
    }

    // время последнего доступа к файлу
    printf("A_TIME                  %s\n", buf);
    if( !strftime(buf, sizeof(buf), fmt, gmtime_r(&sb.st_mtime, &curr_time)) ) {
        exit(EXIT_FAILURE);
    }
    
    // время последней модификации (изменения) файла, изменяется после того как изменяется содержимое файла
    printf("M_TIME                  %s\n", buf);

    return 0;
}