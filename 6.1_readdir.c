// gcc -Werror -Wall -Wextra -Wnarrowing -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast 6.1_readdir.c

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

char dtype_char(unsigned d_type) {
    switch(d_type) {
        case DT_BLK: return 'b'; //блочное устройство
        case DT_CHR: return 'c'; //символьное устройство
        case DT_DIR: return 'd'; //директория
        case DT_FIFO: return 'p'; //именованный канал
        case DT_LNK: return 'l'; //символическая ссылка
        case DT_REG: return '-'; //обычный файл
        case DT_SOCK: return 's'; //сокет
    }
    return '?'; //неизвестный тип (DT_UNKNOWN)
}

//same, но для lstat
char mode_char(mode_t st_mode) {
    switch (st_mode & S_IFMT) {
        case S_IFBLK: return 'b'; 
        case S_IFCHR: return 'c';
        case S_IFDIR: return 'd'; 
        case S_IFIFO: return 'p'; 
        case S_IFLNK: return 'l'; 
        case S_IFREG: return '-'; 
        case S_IFSOCK: return 's';	
    }
    return '?';
}


int main(void) {

    int status = 0;

    //открываем текущую директорию, opendir возвращает указатель на директрию или NULL при ошибке
    DIR* cur_dir = opendir(".");

    //проблемы с открытием
    if(cur_dir == NULL) {
        perror("Can't open directory\n");
        status = 1;
    }

    struct dirent* entry;
    int errno = 0;

    //читаем до конца или до ошибки
    while(1) 
    {
        //readdir возвращает структуру dirent, считанную из файла-директории
        //при достижении конца списка файлов в директории или возникновении ошибки возвращает NULL
        entry = readdir(cur_dir); 
        if(entry == NULL) {
            break;
        }

        //поле d_type отвечает за тип файла
        char file_type = dtype_char(entry->d_type);

        //в случае, когда readdir не смог распознать тип файла, пользуемся lstat
        if(file_type == '?') {
            struct stat sb;
            if(lstat(entry->d_name, &sb) == -1) {
                perror("Error in lstat\n");
                status = 2;
                //если все совсем плохо, прыгаем дальше
                continue;
            }
            file_type = mode_char(sb.st_mode);
        }

        //поле d_name - название файла
        printf("%c %s\n", file_type, entry->d_name);
    }

    //проверяем, вышли ли мы из while из-за достижения конца директории или из-за ошибки в readdir
    if(errno != 0) {
        perror("Error in readdir\n");
        status = 3;
    }

    //ошибка при закрытии
    if(closedir(cur_dir) == -1) {
        perror("Error in closedir\n");
        status = 4;
    }

    return status;
}