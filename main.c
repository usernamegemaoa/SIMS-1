#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "project.h"

void manage_information();
void manage_score();
void statistic();
void system_setting();
static void login_database(const char* user_name, const char *password);

MYSQL sql_connection;

int main()
{
    /* char user_name[41]; */
    /* char password[21]; */

    printf("欢迎使用xx大学学生信息管理系统\n");
    /* printf("请输入用户名： "); */
    /* fgets_remove_newline(user_name, 41, stdin); */
    /* printf("请输入密码: "); */
    /* fgets_remove_newline(password, 21, stdin); */
    /* printf("The user_name is %s, password is %s\n", user_name, password); */

    /* login_database(user_name, password); */
    login_database("mysql", "mysql");

    int option = -1; //initialize with a invalid value
    while(1)
    {
        printf("1):学生基本资料管理\n"
               "2):学生成绩管理\n"
               "3):统计查询\n"
               "4):系统设置\n"
               "5):退出系统\n");
        printf("请选择合适的选项(输入相应的数字): ");
        scanf("%d", &option);
        char c;
        while((c = getchar()) != '\n' && c != EOF)
            ; /*discard the input buffer*/
        switch(option)
        {
            case 1:
                manage_information();
                break;
            case 2:
                manage_score();
                break;
            case 3:
                statistic();
                break;
            case 4:
                system_setting();
                break;
            case 5:
                printf("再见！欢迎下次继续使用该系统\n");
                mysql_close(&sql_connection);
                exit (EXIT_SUCCESS);
            default:
                printf("输入有误，请重试\n");
                break;
        }
    }

    return 0;
}

static void login_database(const char* user_name, const char* password)
{
    mysql_init(&sql_connection);
    if(!mysql_real_connect(&sql_connection, "127.0.0.1", user_name,
                    password, "CExperiment", 0, NULL, 0))
    {
        fprintf(stderr, "login database failed\n");
        fprintf(stderr,"%s\n", mysql_error(&sql_connection));
        exit (EXIT_FAILURE);
    }
    if(mysql_set_character_set(&sql_connection, "utf8"))
        fprintf(stderr, "set character-set occurs error\n");
}
