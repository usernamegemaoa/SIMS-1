#include <stdio.h>
#include <string.h>
#include <mysql.h>
#include "project.h"

static void add_information();
static void update_information();
static void delete_information();

void manage_information()
{
    int option= -1; //initialize with a invalid value
    while(1)
    {
        printf("进入学生基本信息管理界面\n");
        printf("1):添加学生信息\n"
               "2):修改学生信息\n"
               "3):删除学生信息\n"
               "4):返回上一级\n");
        printf("请选择合适的选项(输入相应的数字): ");
        scanf("%d", &option);
        clean_input_stream();
        switch(option)
        {
            case 1:
                add_information();
                break;
            case 2:
                update_information();
                break;
            case 3:
                delete_information();
                break;
            case 4:
                return;
            default:
                printf("输入有误，请重新输入\n");
                break;
        }
    }
}

static void add_information()
{
    char id[11];
    char name[15];
    char sex[4];
    char class[4];
    char department[25];
    char phone_number[12];
    char family_address[31];
    char *mysql_insert = "INSERT INTO student VALUES(";
    //size =the sum of the above array's length
    char insert_statement[strlen(mysql_insert)
        +sizeof(id)+sizeof(name)+sizeof(sex)
        +sizeof(class)+sizeof(department)
        +sizeof(phone_number)+sizeof(family_address)];

    printf("请输入学生信息\n学号: ");
    fgets_remove_newline(id, 11, stdin);
    printf("姓名: ");
    fgets_remove_newline(name, 15, stdin);
    printf("性别(男/女): ");
    fgets_remove_newline(sex, 4, stdin);
    printf("班级: ");
    fgets_remove_newline(class, 4, stdin);
    printf("系名: ");
    fgets_remove_newline(department, 25, stdin);
    printf("联系电话: ");
    fgets_remove_newline(phone_number, 12, stdin);
    printf("籍贯: ");
    fgets_remove_newline(family_address, 31, stdin);

    //INSERT INTO student VALUES('id','name','sex','class',
    //'department','phone_number','family_address')
    snprintf(insert_statement, sizeof(insert_statement),
            "%s'%s','%s','%s','%s','%s','%s','%s')",
            mysql_insert, id, name, sex, class,
            department, phone_number, family_address);

    if(mysql_query(&sql_connection, insert_statement))
    {
        fprintf(stderr,"INSERT error %d: %s\n", mysql_errno(&sql_connection),
                mysql_error(&sql_connection));
        return;
    }
    else
    {
        printf("Successfully insert student information:\n");
        char select_statement[45];
        select_statement[0] = 0;
        strcat(select_statement, "SELECT * FROM student WHERE 学号 = ");
        strcat(select_statement, id);
        if(mysql_query(&sql_connection, select_statement))
        {
            fprintf(stderr,"Error %d: %s\n", mysql_errno(&sql_connection),
                    mysql_error(&sql_connection));
            return;
        }

        MYSQL_RES *res_ptr = mysql_store_result(&sql_connection);
        if(!res_ptr)
            fprintf(stderr,"Error %d: %s\n", mysql_errno(&sql_connection),
                    mysql_error(&sql_connection));
        MYSQL_ROW row = mysql_fetch_row(res_ptr);
        unsigned int num_fields = mysql_num_fields(res_ptr);
        for(unsigned int i = 0; i < num_fields; i++)
            printf(" %s ", row[i] ? row[i] : "NULL");
        mysql_free_result(res_ptr);
        printf("\n");
    }
}

static void update_information()
{
    printf("update information\n");
}

static void delete_information()
{
    printf("delete information\n");
}
