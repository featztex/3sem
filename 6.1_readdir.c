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

/*
    struct dirent
    {
        long d_ino; номер индекса inode
        　　
        off_t d_off; смещение к этому прямому смещению в файле каталога
        　　
        unsigned short d_reclen; длина этого d_name
        　　
        unsigned char d_type; тип файла типа d_name
        　　
        char d_name [NAME_MAX+1]; имя файла (с нулевым символом в конце) Имя файла, до 255 символов 
    }
*/

int main(void) {

    int status = 0;

    //открываем текущую директорию, opendir возвращает указатель на директрию или NULL при ошибке
    DIR* cur_dir = opendir(".");

    //проблемы с открытием
    if(cur_dir == NULL) {
        perror("Can't open directory");
        closedir(cur_dir);
        status = 1;
    }

    //читаем до конца или до ошибки
    while(1) 
    {
        errno = 0;
        struct dirent* entry;

        //readdir возвращает структуру dirent, считанную из файла-директории
        //при достижении конца списка файлов в директории или возникновении ошибки возвращает NULL
        entry = readdir(cur_dir); 
        if(entry == NULL)
            break;

        //тип файла
        char file_type = dtype_char(entry->d_type);

        //в случае, когда readdir не смог распознать тип файла, пользуемся lstat
        if(file_type == '?') {
            struct stat sb;
            if(lstat(entry->d_name, &sb) == -1) {
                //если все совсем плохо, прыгаем дальше
                perror("Error in lstat");
                status = 2;
                continue;
            }
            file_type = mode_char(sb.st_mode);
        }

        printf("%c %s\n", file_type, entry->d_name);
    }

    //проверяем из-за чего мы вышли из while: из-за достижения конца директории или из-за ошибки в readdir
    if(errno != 0) {
        perror("Error in readdir");
        status = 3;
    }

    //ошибка при закрытии
    if(closedir(cur_dir) == -1) {
        perror("Error in closedir");
        status = 4;
    }

    return status;
}