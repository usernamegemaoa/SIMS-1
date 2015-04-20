/*
 * 记得把execl中的mysql改为软编码，即将main.c中的user_name
 * 包含到project.h中
 */
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include "project.h"

#define CURRENT_USER "mysql"
#define MAX_USER_NAME 16
#define MAX_PASSWORD  16

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
    char user_name[MAX_USER_NAME + 1];
    //new password
    char password[MAX_PASSWORD + 1];

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
                    char answer = getchar();
                    clean_input_stream();
                    if(toupper(answer) != 'Y')
                        break;
                    printf("请输入用户名: ");
                    fgets_remove_newline(user_name, sizeof(user_name), stdin);
                    printf("请输入新密码: ");
                    fgets_remove_newline(password, sizeof(password), stdin);
                    //set password for 'username'@'localhost' = password('pw')
                    char sql_set_passwd[46 + MAX_USER_NAME + MAX_PASSWORD + 1];
                    sprintf(sql_set_passwd,
                        "SET PASSWORD FOR '%s'@'localhost' = PASSWORD('%s')",
                        user_name[0] ? user_name : CURRENT_USER,
                        password);
                    if(mysql_query(&sql_connection, sql_set_passwd)) {
                        fprintf(stderr, "密码修改失败\n%s\n",
                                mysql_error(&sql_connection));
                    } else
                        printf("成功修改%s的密码\n",
                                user_name[0] ? user_name : CURRENT_USER);
                }
                break;
            case 2:
                {
                    //create user 'username'@'localhost' identified by 'passwd'
                    char sql_create_user[43 + MAX_USER_NAME + MAX_PASSWORD + 1];
                    //grant all on Cexperiment.* TO 'username'@'localhost'
                    char sql_grant[44 + MAX_USER_NAME + 1];

                    printf("注意: 需要管理员权限才能添加用户,是否继续?(y/n): ");
                    char answer = getchar();
                    clean_input_stream();
                    if(toupper(answer) != 'Y')
                        break;
                    printf("请输入用户名(长度不大于%d): ", MAX_USER_NAME);
                    fgets_remove_newline(user_name, sizeof(user_name), stdin);
                    printf("请输入新用户(%s)的密码(长度不大于%d): ",
                            user_name, MAX_PASSWORD);
                    fgets_remove_newline(password, sizeof(password), stdin);
                    sprintf(sql_create_user,
                            "CREATE USER '%s'@'localhost' IDENTIFIED BY '%s'",
                            user_name, password);
                    sprintf(sql_grant,
                            "GRANT ALL ON CExperiment.* TO '%s'@'localhost'",
                            user_name);
                    if(mysql_query(&sql_connection, sql_create_user)
                            || mysql_query(&sql_connection, sql_grant)) {
                        fprintf(stderr, "用户添加失败\n%s\n",
                                mysql_error(&sql_connection));
                    } else
                        printf("用户添加成功\n");
                }
                break;
            case 3:
                {
                    //drop user 'username'@'localhost'
                    char sql_drop_user[24 + MAX_USER_NAME + 1];
                    printf("注意: 需要管理员权限才能删除用户,是否继续？(y/n): ");
                    char answer = getchar();
                    clean_input_stream();
                    if(toupper(answer) != 'Y')
                        break;
                    printf("请输入用户名: ");
                    fgets_remove_newline(user_name, sizeof(user_name), stdin);
                    sprintf(sql_drop_user, "DROP USER '%s'@'localhost'",
                            user_name);
                    if(mysql_query(&sql_connection, sql_drop_user)) {
                        fprintf(stderr, "删除用户失败\n%s\n",
                                mysql_error(&sql_connection));
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
    printf("该操作无法撤销，确定清空该数据库中的所有信息？(y/n): ");
    char answer = getchar();
    clean_input_stream();
    if(toupper(answer) != 'Y')
        return;
    mysql_autocommit(&sql_connection, 0);
    mysql_query(&sql_connection, "DELETE FROM course");
    mysql_query(&sql_connection, "DELETE FROM student");
    mysql_query(&sql_connection, "DELETE FROM grade");
    if(mysql_commit(&sql_connection)) {
        fprintf(stderr, "初始化失败\n%s\n", mysql_error(&sql_connection));
        if(mysql_rollback(&sql_connection))
            fprintf(stderr, "回滚失败\n%s\n", mysql_error(&sql_connection));
    } else
        printf("成功初始化数据库\n");
    mysql_autocommit(&sql_connection, 1);
}

static void backup_database()
{
    pid_t pid;
    char backup_file[100];
    printf("请输入备份文件名: ");
    fgets_remove_newline(backup_file, sizeof(backup_file), stdin);
    //test for existence of backup_file
    if(access(backup_file, F_OK) == 0) {
        fprintf(stderr, "%s文件已存在，是否覆盖原文件？(y/n)", backup_file);
        char answer = getchar();
        clean_input_stream();
        if(toupper(answer) != 'Y') {
            fprintf(stderr, "备份已取消\n");
            return;
        }
    }

    if((pid = fork()) < 0) {
        perror("备份失败");
        return;
    } else if(pid) { //parent
        waitpid(pid, NULL, 0);
        printf("备份成功\n");
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
            execl("/usr/bin/mysqldump", "mysqldump", "-u", CURRENT_USER,
                    "-p", "--databases", "CExperiment", (char *)0);
        } else { //grandson
            close(0);
            close(1);
            dup(pipefd[0]); //redirect stdin
            close(pipefd[0]);
            close(pipefd[1]);
            //return file desciptor 1(stdout)
            open(backup_file, O_WRONLY | O_CREAT | O_TRUNC,
                    S_IRUSR | S_IWUSR | S_IRGRP);
            execl("/bin/gzip", "gzip", (char *)0);
        }
    }
}

static void restore_database()
{
    pid_t pid;
    char backup_file[100];
    printf("请输入用于恢复数据库的文件名: ");
    fgets_remove_newline(backup_file, sizeof(backup_file), stdin);
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
        waitpid(pid, NULL, 0);
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
            execl("/bin/gunzip", "gunzip", "-c", backup_file, (char *)0);
        } else { //grandson
            /*
             * restore database
             * redirect stdin to pipefd[0] for mysql program
             */
            close(0);
            dup(pipefd[0]);
            close(pipefd[0]);
            close(pipefd[1]);
            execl("/usr/bin/mysql", "mysql", "-u", CURRENT_USER, "-p",
                    "CExperiment", (char *)0);
        }
    }
}
