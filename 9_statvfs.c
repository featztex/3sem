//gcc -Werror -Wall -Wextra -Wnarrowing -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast 9_statvfs.c

#include <stdio.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/statvfs.h>

/*
определение структуры statvfs: 
    unsigned long  f_bsize;     размер блока файловой системы 
    unsigned long  f_frsize;    размер фрагмента 
    fsblkcnt_t     f_blocks;    размер ФС в единицах f_frsize 
    fsblkcnt_t     f_bfree;     количество свободных блоков 
    fsblkcnt_t     f_bavail;    количество свободных блоков
                                для непривилегированных пользователей 
    fsfilcnt_t     f_files;     количество inodes 
    fsfilcnt_t     f_ffree;     количество свободных inodes 
    fsfilcnt_t     f_favail;    количество свободных inodes
                                для непривилегированных пользователей
    unsigned long  f_fsid;      идентификатор файловой системы
    unsigned long  f_flag;      параметры монтирования 
    unsigned long  f_namemax;   максимальная длина имени файла 
*/

int main(int argc, char *argv[])
{
    //некорректное число аргументов
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <pathname> \n", argv[0]);
        return 1;
    }

    struct statvfs sv;
    
    //в случае успеха statvfs возвращает 0, иначе -1
	if (statvfs(argv[1], &sv) == -1)
    {
        perror("Failed to statfs");
        return 2;
    }

    //всего
    printf("Total (bytes): %ju\n", sv.f_blocks * (__uintmax_t)sv.f_frsize);
    //свободно для суперюзера
    printf("Free (bytes): %ju\n", sv.f_bfree * (__uintmax_t)sv.f_frsize);
    //свободно для непривилигерованных юзеров
    printf("Free for unprivileged users (bytes): %ju\n", sv.f_bavail * (__uintmax_t)sv.f_frsize);
    //использовано
    printf("Used (bytes): %ju\n", (sv.f_blocks - sv.f_bfree) * (__uintmax_t)sv.f_frsize);
    return 0;
}