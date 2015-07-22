#include "project.h"

static void manage_user(void);
static void backup_database(void);
static void restore_database(void);
static void init_database(void);

void system_setting()
{
    while(1) {
        int option = -1; //initialize with a invalid value
        printf("    + + + + + + + + + + + + + + +\n"
               "    +    欢迎进入系统设置界面   +\n"
               "    +                           +\n"
               "    +      1):用户管理          +\n"
               "    +      2):初始化数据库      +\n"
               "    +      3):备份数据库        +\n"
               "    +      4):恢复数据库        +\n"
               "    +      5):返回上一级        +\n"
               "    + + + + + + + + + + + + + + +\n");
        printf("请选择合适的选项(输入相应的数字): ");
        scanf("%d", &option);
        clean_input_stream();
        switch(option) {
            case 1:
                 manage_user();
                break;
            case 2:
                init_database();
                break;
            case 3:
                backup_database();
                break;
            case 4:
                restore_database();
                break;
            case 5:
                return;
            default:
                printf("输入有误，请重新输入\n");
                break;
        }
    }
}

static void manage_user()
{
    //the user name to add or delete
    char user_name[USER_NAME_LEN + 1];
    char new_passwd[PASSWD_LEN + 1];

    while(1) {
        int option = -1; //initialized with a invalid value
        printf("    + + + + + + + + + + + + + + +\n"
               "    +    欢迎进入用户管理界面   +\n"
               "    +                           +\n"
               "    +      1):修改密码          +\n"
               "    +      2):添加用户          +\n"
               "    +      3):删除用户          +\n"
               "    +      4):返回上一级        +\n"
               "    + + + + + + + + + + + + + + +\n");
        printf("请选择合适的选项(输入相应的数字): ");
        scanf("%d", &option);
        clean_input_stream();
        switch(option) {
            case 1:
                {
                    printf("要求输入用户名时直接回车表示修改自己的密码\n"
                           "注意: 需要管理员权限才能修改其他用户的密码\n"
                           "是否继续？(y/n): ");
                    if(is_no())
                        break;
                    printf("请输入要修改密码的用户名: ");
                    Fgets_stdin(user_name, sizeof(user_name));
                    printf("请输入新密码(最长为%d位): ", PASSWD_LEN);
                    get_passwd(new_passwd, sizeof(new_passwd));
                    char sql_set_passwd[strlen("SET PASSWORD FOR ''")
                        + strlen(user_name)
                        + strlen("@'localhost' = PASSWORD('')")
                        + PASSWD_LEN + 1];
                    sprintf(sql_set_passwd,
                        "SET PASSWORD FOR '%s'@'localhost' = PASSWORD('%s')",
                        user_name[0] ? user_name : current_user,
                        new_passwd);
                    if(mysql_query(mysql, sql_set_passwd)) {
                        fprintf(stderr, "密码修改失败\n%s\n",
                                mysql_error(mysql));
                    } else
                        printf("成功修改%s的密码\n",
                                user_name[0] ? user_name : current_user);
                }
                break;
            case 2:
                {
                    printf("注意: 需要管理员权限才能添加用户,"
                            "是否继续?(y/n): ");
                    if(is_no())
                        break;
                    printf("请输入用户名(最长为%d位): ", USER_NAME_LEN);
                    Fgets_stdin(user_name, sizeof(user_name));
                    printf("请输入新用户(%s)的密码(最长为%d位): ",
                            user_name, PASSWD_LEN);
                    get_passwd(new_passwd, sizeof(new_passwd));

                    char sql_create_user[strlen("CREATE USER ''")
                        + strlen(user_name)
                        + strlen("@'localhost' IDENTIFIED BY ''")
                        + strlen(new_passwd) + 1];
                    sprintf(sql_create_user,
                            "CREATE USER '%s'@'localhost' IDENTIFIED BY '%s'",
                            user_name, new_passwd);
                    char sql_grant[strlen("GRANT ALL ON ")
                        + strlen(DATABASE) + strlen(".* TO ''")
                        + strlen(user_name) + strlen("@'localhost'") + 1];
                    sprintf(sql_grant,
                            "GRANT ALL ON %s.* TO '%s'@'localhost'",
                            DATABASE, user_name);
                    if(mysql_query(mysql, sql_create_user)
                            || mysql_query(mysql, sql_grant)) {
                        fprintf(stderr, "用户添加失败\n%s\n",
                                mysql_error(mysql));
                    } else
                        printf("用户添加成功\n");
                }
                break;
            case 3:
                {
                    //drop user 'username'@'localhost'
                    printf("注意: 需要管理员权限才能删除用户,"
                            "是否继续?(y/n): ");
                    if(is_no()) {
                        printf("取消删除用户\n");
                        break;
                    }
                    printf("请输入用户名: ");
                    Fgets_stdin(user_name, sizeof(user_name));
                    char sql_drop_user[strlen("DROP USER ''")
                        + strlen(user_name) + strlen("@'localhost'") + 1];
                    sprintf(sql_drop_user, "DROP USER '%s'@'localhost'",
                            user_name);
                    if(mysql_query(mysql, sql_drop_user)) {
                        fprintf(stderr, "删除用户失败\n%s\n",
                                mysql_error(mysql));
                    } else
                        printf("成功删除%s\n", user_name);
                }
                break;
            case 4:
                return;
            default:
                printf("输入有误，请重新输入\n");
                break;
        }
    }
}

static void init_database()
{
    printf("该操作无法撤销，确定清空该数据库中的所有信息?(y/n): ");
    if(is_no()) {
        printf("取消清空数据库\n");
        return;
    }
    //初始化有问题，有些表没有成功删除信息
    if(mysql_autocommit(mysql, 0)) {
        fprintf(stderr, "初始化失败,无法修改自动提交设置\n");
        return;
    }
    mysql_query(mysql, "DELETE FROM course");
    mysql_query(mysql, "DELETE FROM grade");
    mysql_query(mysql, "DELETE FROM student");
    if(mysql_commit(mysql)) {
        fprintf(stderr, "初始化失败\n%s\n", mysql_error(mysql));
        if(mysql_rollback(mysql))
            fprintf(stderr, "回滚失败\n%s\n", mysql_error(mysql));
    } else
        printf("成功初始化数据库\n");
    mysql_autocommit(mysql, 1);
}

static void backup_database()
{
    pid_t pid;
    char backup_file[100];
    printf("请输入备份文件名: ");
    Fgets_stdin(backup_file, sizeof(backup_file));
    //test for existence of backup_file
    if(access(backup_file, F_OK) == 0) {
        fprintf(stderr, "%s文件已存在，是否覆盖原文件？(y/n)", backup_file);
        if(is_no()) {
            fprintf(stderr, "取消备份\n");
            return;
        }
    }

    if((pid = fork()) < 0) {
        perror("备份失败");
        return;
    } else if(pid) { //parent
        int status = 0;
        if(wait(&status) > 0) { // wait on success
            //even if wait on success, if exit status is non-zero,
            //there was a error produced by mysqldump.
            if(WIFEXITED(status) && !WEXITSTATUS(status))
                printf("备份成功\n");
            else
                printf("备份失败\n");
        } else //wait on error
            perror("备份失败");
    } else { //child
        pid_t pipefd[2];
        if(pipe(pipefd)) {
            perror("备份失败");
            return;
        }
        if((pid = fork()) < 0) {
            perror("备份失败");
            return;
        } else if(pid) {
            close(1);    //close stdout
            dup(pipefd[1]); //redirect stdout to pip[1]
            close(pipefd[0]);
            close(pipefd[1]);
            close(2);
            char passwd_option[strlen("-p") + strlen(passwd) + 1];
            sprintf(passwd_option, "-p%s", passwd);
            if(execl("/usr/bin/mysqldump", "mysqldump", "-u", current_user,
                    passwd_option, "--databases", DATABASE, (char *)0)) {
                perror("无法备份");
                exit(EXIT_FAILURE);
            }
        } else { //grandson
            close(0);
            close(1);
            dup(pipefd[0]); //redirect stdin
            close(pipefd[0]);
            close(pipefd[1]);
            //return file desciptor 1(stdout)
            open(backup_file, O_WRONLY | O_CREAT | O_TRUNC,
                    S_IRUSR | S_IWUSR | S_IRGRP);
            if(execl("/bin/gzip", "gzip", (char *)0)) {
                perror("备份压缩失败");
                exit(EXIT_FAILURE);
            }
        }
    }
}

static void restore_database()
{
    pid_t pid;
    char backup_file[100];
    printf("请输入用于恢复数据库的文件名: ");
    Fgets_stdin(backup_file, sizeof(backup_file));
    if(access(backup_file, F_OK)) {
        perror("恢复失败");
        return;
    }
    if(access(backup_file, R_OK)) {
        perror("恢复失败");
        return;
    }

    if((pid = fork()) < 0) {
        perror("恢复失败");
        return;
    } else if(pid) {  //parent
        int status = 0;
        if(wait(&status) > 0) { // wait on success
            if(WIFEXITED(status) && !WEXITSTATUS(status))
                printf("恢复成功\n");
            else
                printf("恢复失败\n");
        } else //wait on error
            perror("恢复失败");
    } else {  //child
        pid_t pipefd[2];
        if(pipe(pipefd)) {
            perror("恢复失败");
            return;
        }
        if((pid = fork()) < 0) {
            perror("恢复失败");
            return;
        } else if(pid) {
            /*
             * redirect stdout to pipefd[1]
             * using gunzip decompresses backup_file
             * and write output on stdout(pipefd[1])
             */
            close(1);
            dup(pipefd[1]); //redirect stdout to pipefd[1]
            close(pipefd[0]);
            close(pipefd[1]);
            if(execl("/bin/gunzip", "gunzip", "-c", backup_file, (char *)0))
                perror("文件格式有误\n");
        } else { //grandson
            /*
             * restore database
             * redirect stdin to pipefd[0] for mysql program
             */
            close(0);
            dup(pipefd[0]);
            close(pipefd[0]);
            close(pipefd[1]);
            close(2);
            char passwd_option[strlen("-p") + strlen(passwd) + 1];
            sprintf(passwd_option, "-p%s", passwd);
            execl("/usr/bin/mysql", "mysql", "-u", current_user,
                    passwd_option, DATABASE, (char *)0);
        }
    }
}
