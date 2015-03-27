#include <stdio.h>
#include <string.h>
#include <ctype.h>
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
        printf("欢迎进入学生基本信息管理界面\n");
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
    sprintf(insert_statement,
            "%s'%s','%s','%s','%s','%s','%s','%s')",
            mysql_insert, id, name, sex, class,
            department, phone_number, family_address);

    if(mysql_query(&sql_connection, insert_statement))
        fprintf(stderr,"insert failed: %s\n", mysql_error(&sql_connection));
    else
    {
        printf("Successfully insert student information\n");
        printf("学号: %s, 姓名: %s, 性别: %s, 班级: %s, 系: %s,联系电话: %s, 籍贯: %s\n",
                id, name, sex, class, department, phone_number, family_address);
    }
}

static void update_information()
{
    char id[11];
    char select_statement[50] = "SELECT * FROM student WHERE 学号 = ";

    while(1)
    {
        printf("请输入要修改信息的学生的学号: ");
        fgets_remove_newline(id, 11, stdin);
        strcat(select_statement, id);

        if(mysql_query(&sql_connection, select_statement))
            fprintf(stderr, "没有学号为%s的学生\n", id);
        else
        {
            MYSQL_RES *result = mysql_store_result(&sql_connection);
            if(!result)
                fprintf(stderr,"%s\n", mysql_error(&sql_connection));
            MYSQL_ROW row = mysql_fetch_row(result);
            MYSQL_FIELD *fields = mysql_fetch_fields(result);
            unsigned int num_fields = mysql_num_fields(result);
            unsigned long *field_lengths = mysql_fetch_lengths(result);
            //21 is the largest length of fields(籍贯)
            char new_info[num_fields][21];
            printf("对于每一项信息，如果要修改则输入新的信息，否则直接回车跳过相应的项\n");
            for(unsigned int i = 0; i < num_fields; ++i)
            {
                printf("%s[%s,l:%ld]: ", fields[i].name, row[i] ? row[i] : "NULL",
                        field_lengths[i]);
                fgets_remove_newline(new_info[i], field_lengths[i] + 1, stdin);
            }
            mysql_free_result(result);

            //construct sql update statement
            //safe size = (about)150 = the sum of the length of all fields and their names
            //20,6,10,6,13,6,3,6,3,3,20,12,11,6,25
            char update_statement[160] = "UPDATE student set ";
            for(unsigned int i = 0; i < num_fields; ++i)
            {
                if(new_info[i][0] != 0)
                {
                    char col_name_value_pair[fields[i].name_length
                                           + field_lengths[i] + 1];
                    sprintf(col_name_value_pair, "%s='%s',",
                            fields[i].name, new_info[i]);
                    printf("col_name_value_pair is '%s'\n", col_name_value_pair);
                    strcat(update_statement, col_name_value_pair);
                }
            }
            //delete the trailing comma
            update_statement[strlen(update_statement) - 1] = 0;

            if(mysql_query(&sql_connection, update_statement))
                fprintf(stderr,"update failed: %s\n", mysql_error(&sql_connection));
            else
                printf("update successfully\n");
        }

        printf("继续修改学生信息？(y/n): ");
        char answer = getchar();
        clean_input_stream();
        if(toupper(answer) != 'Y')
            break;
    }
}

static void delete_information()
{
    printf("delete information\n");
}
