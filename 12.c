//gcc -Werror -Wall -Wextra -Wnarrowing -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast 12.c

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <grp.h>
#include <pwd.h>
#include <string.h>
#include <stdlib.h>


int main(void) {

    /* 
    getpid() - ID текущего процесса
    getppid() - ID родительского процесса
    getpgrp() - ID группы процессов текущего процесса
    getsid(0) - ID сессии 
    */

    printf("PID %d, PPID %d, PGID %d, SID %d\n", (pid_t)getpid(), (pid_t)getpid(), (pid_t)getpgrp(), (pid_t)getsid(0));

    // ID реального пользователя и процесса
    // Эти ID определяют кто владелец процесса
    uid_t ruid = getuid(); // фактический ID пользователя вызывающего процесса
    gid_t rgid = getgid(); // действительный идентификатор группы вызывающего процесса

    const char* qstn = "?";

    /*
        struct passwd {
            char   *pw_name;       имя пользователя 
            char   *pw_passwd;     пароль пользователя
            uid_t   pw_uid;        идентификатор пользователя
            gid_t   pw_gid;        идентификатор группы
            char   *pw_gecos;      информация о пользователе
            char   *pw_dir;        домашний каталог 
            char   *pw_shell;      программная оболочка 
        };
    */

    struct passwd* rpwuid = getpwuid(ruid);
    const char* rpw_name = rpwuid ? rpwuid->pw_name : qstn; // если в pw_name лежит NULL, имя = ?

    struct group* rgrid = getgrgid(rgid);
    const char* rgr_name = rgrid ? rgrid->gr_name : qstn;

    printf("UID %d (%s), GID %d (%s)\n", ruid, rpw_name, rgid, rgr_name);

    // ID эффективного пользователя и группы
    // Эти ID используются ядром для определения прав, которые будет иметь процесс при доступе к общим ресурсам
    uid_t euid = geteuid();
    gid_t egid = getegid();

    struct passwd * epwuid = getpwuid(euid);
    const char* epw_name = epwuid ? epwuid->pw_name : qstn;

    struct group * egrid = getgrgid(egid);
    const char* egr_name = egrid ? egrid->gr_name : qstn;

    printf("EUID %d(%s), EGID %d(%s)\n", euid, epw_name, egid, egr_name);

    //узнаем число групп
    int groups_num = getgroups(0, NULL);
    if(groups_num < 0) {
        perror("Error in getgroups");
        return 1;
    }
    gid_t groups_list[groups_num];
    groups_num = getgroups(groups_num, groups_list);
    printf("Number of groups (%d): ", groups_num);

    for(int i = 0; i < groups_num; ++i) {
        printf("%d", groups_list[i]);
        // getgrgid() возвращает указатель на структуру, содержащую информацию о группе
        struct group* group_info = getgrgid(groups_list[i]);
        if (group_info == NULL) 
            printf("(?)"); 
        else
            printf("(%s) ", group_info->gr_name);
    }

    printf("\n");

    return 0;
}