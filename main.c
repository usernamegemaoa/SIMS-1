#include "project.h"
#include <termios.h>

extern void manage_information();
extern void manage_grade();
extern void statistic();
extern void system_setting();
static void login_database(const char* user_name, const char *password);
static void get_passwd(char *dest, int size);

MYSQL *mysql;
char current_user[USER_NAME_LEN + 1];
char passwd[PASSWD_LEN + 1];

int main()
{

    printf("请输入用户名： ");
    Fgets_stdin(current_user, sizeof(current_user));
    printf("请输入密码: ");
    get_passwd(passwd, sizeof(passwd));
    printf("\n");
    login_database(current_user, passwd);

    while(1)
    {
        int option = -1; //initialize with a invalid value
        printf("    + + + + + + + + + + + + + + + + + + +\n"
               "    +   欢迎使用xx大学学生信息管理系统  +\n"
               "    +                                   +\n"
               "    +     1):学生基本信息管理           +\n"
               "    +     2):学生成绩管理               +\n"
               "    +     3):统计查询                   +\n"
               "    +     4):系统设置                   +\n"
               "    +     5):退出系统                   +\n"
               "    + + + + + + + + + + + + + + + + + + +\n");
        printf("请选择合适的选项(输入相应的数字): ");
        scanf("%d", &option);
        clean_input_stream();
        switch(option)
        {
            case 1:
                manage_information();
                break;
            case 2:
                manage_grade();
                break;
            case 3:
                statistic();
                break;
            case 4:
                system_setting();
                break;
            case 5:
                printf("再见！欢迎下次继续使用该系统\n");
                mysql_close(mysql);
                exit(EXIT_SUCCESS);
            default:
                printf("输入有误，请重试\n");
                break;
        }
    }

    return 0;
}

static void get_passwd(char *dest, int size)
{
    if(size <= 0) {
        fprintf(stderr, "密码长度至少为1, 请重新设置\n");
        exit(1);
    }
    struct termios old_setting, new_setting;
    if(tcgetattr(0, &old_setting)) {
        perror("获取终端属性出错");
        exit(1);
    }
    new_setting = old_setting;
    new_setting.c_lflag &= ~ECHO;
    if(tcsetattr(0, TCSAFLUSH, &new_setting)) {
        perror("设置终端属性失败");
        exit(1);
    }
    Fgets_stdin(dest, size);
    tcsetattr(0, TCSANOW, &old_setting);
}

static void login_database(const char* user_name, const char* passwd)
{
    mysql = mysql_init(NULL);
    if(mysql == NULL)
    {
        fprintf(stderr, "mysql_init(): can't init\n");
        exit(EXIT_FAILURE);
    }
    if(!mysql_real_connect(mysql, "127.0.0.1", user_name,
                    passwd, DATABASE, 0, NULL, 0))
    {
        fprintf(stderr, "login database failed\n%s\n", mysql_error(mysql));
        exit(EXIT_FAILURE);
    }
    if(mysql_set_character_set(mysql, "utf8"))
        fprintf(stderr, "set character-set occurs error\n");
}
