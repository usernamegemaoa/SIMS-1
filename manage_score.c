#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <mysql.h>
#include "project.h"

static char id[11];

int student_exists(const char* id);
static void add_score();
static void change_score();

void manage_score()
{
    printf("欢迎进入学生成绩管理界面\n");
    while(1)
    {
        int option = -1; //initialize with a invalid value
        printf("1):添加学生成绩\n"
               "2):修改学生成绩\n"
               "3):删除学生成绩\n"
               "4):返回上一级\n");
        printf("请选择合适的选项(输入相应的数字): ");
        scanf("%d", &option);
        clean_input_stream();
        switch(option)
        {
            case 1:
            case 2:
            case 3:
                printf("请输入学生的学号: ");
                fgets_remove_newline(id, 11, stdin);
                if(!student_exists(id))
                    fprintf(stderr, "没有学号为%s的学生\n", id);
                else if(option == 1)
                    add_score();
                else
                    change_score(option);
                break;
            case 4:
                return;
            default:
                printf("输入有误，请重新输入\n");
                break;
        }
    }
}

static void add_score()
{
    char select_statement[100] =
        "SELECT Cname, Cid FROM course WHERE Cid not in"
        "(SELECT Cid FROM grade WHERE 学号 = '";
    strcat(select_statement, id);
    strcat(select_statement, "')");

    if(mysql_query(&sql_connection, select_statement))
        fprintf(stderr, "%s\n", mysql_error(&sql_connection));
    else
    {
        MYSQL_RES *result = mysql_store_result(&sql_connection);
        if(!result)
            fprintf(stderr, "%s\n", mysql_error(&sql_connection));
        else if(!mysql_num_rows(result))
            fprintf(stderr, "该学生所有的课程都已经有成绩了，不能再添加成绩\n");
        else
        {
            MYSQL_ROW row = mysql_fetch_row(result);
            my_ulonglong num_rows = mysql_num_rows(result);
            int course_choice = 0;
            printf("该学生可以添加成绩的课程为:\n");
            if(num_rows == 1)
                printf("%s\n", row[0]);
            else
            {
                for(unsigned int i = 0; i < num_rows; ++i)
                {
                    printf("%d. %s\n", (i + 1), row[0]);
                    row = mysql_fetch_row(result);
                }
                printf("请选择要添加成绩的课程(输入相应的数字): ");
                while(1)
                {
                    scanf("%d", &course_choice);
                    clean_input_stream();
                    if(course_choice < 1 || course_choice > num_rows)
                        printf("输入有误，请重新输入: ");
                    else
                        break;
                }
            }
            int score = -1;
            printf("请输入学生的%s成绩: ", row[0]);
            while(1)
            {
                scanf("%d", &score);
                clean_input_stream();
                if(score < 0 || score > 100)
                    printf("成绩输入超出范围(0-100)，请重新输入: ");
                else
                    break;
            }

            //if course_choice == 1, there is only one row in result
            char *sql_insert_header = "INSERT INTO grade VALUES(";
            char insert_score[strlen(sql_insert_header)+10+4+5];
            char *course_id;
            if(course_choice != 0)
            {
                mysql_data_seek(result, course_choice - 1);
                row = mysql_fetch_row(result);
            }
            course_id = row[1];

            sprintf(insert_score, "%s'%s','%s','%d')",
                    sql_insert_header, id, course_id, score);
            if(mysql_query(&sql_connection, insert_score))
                fprintf(stderr, "%s\n", mysql_error(&sql_connection));
            else
                printf("成功添加学生成绩\n\n");
        }
        mysql_free_result(result);
    }
}

/*
 * change student's score
 * if option == 2, update score,
 * otherwise, delete score
 */
static void change_score(int option)
{
    char *delete_or_update[2] = {"修改", "删除"};
    char select_statement[125] =
        "SELECT Cname, course.Cid, grade FROM course, grade WHERE"
        " course.Cid = grade.Cid"
        " AND grade.学号='";
    strcat(select_statement, id);
    strcat(select_statement, "'");

    if(mysql_query(&sql_connection, select_statement))
        fprintf(stderr, "%s\n", mysql_error(&sql_connection));
    else
    {
        MYSQL_RES *result = mysql_store_result(&sql_connection);
        if(!result)
            fprintf(stderr, "%s\n", mysql_error(&sql_connection));
        else if(!mysql_num_rows(result))
            fprintf(stderr, "该学生所有的课都没有成绩，无法进行%s\n",
                    delete_or_update[option - 2]);
        else
        {
            MYSQL_ROW row = mysql_fetch_row(result);
            my_ulonglong num_rows = mysql_num_rows(result);
            int course_choice = 0;
            printf("该学生有成绩的课程为:\n");
            if(num_rows == 1)
                printf("%s %s分\n", row[0], row[2]);
            else
            {
                for(unsigned int i = 0; i < num_rows; ++i)
                {
                    printf("%d. %s %s分\n", (i + 1), row[0], row[2]);
                    row = mysql_fetch_row(result);
                }
                printf("请选择要%s成绩的课程(输入相应的数字): ",
                        delete_or_update[option - 2]);
                while(1)
                {
                    scanf("%d", &course_choice);
                    clean_input_stream();
                    if(course_choice < 1 || course_choice > num_rows)
                        printf("输入有误，请重新输入: ");
                    else
                        break;
                }
                mysql_data_seek(result, course_choice - 1);
                row = mysql_fetch_row(result);
            }
            char *Cid = row[1];
            //update score
            if(option == 2)
            {
                int score = -1;
                printf("请输入学生的%s成绩: ", row[0]);
                while(1)
                {
                    scanf("%d", &score);
                    clean_input_stream();
                    if(score < 0 || score > 100)
                        printf("成绩输入超出范围(0-100)，请重新输入: ");
                    else
                        break;
                }
                char *update_score_header = "UPDATE grade SET grade = ";
                char update_score[strlen(update_score_header) + 31];
                sprintf(update_score, "%s'%d' WHERE 学号 = '%s' AND Cid = '%s'",
                        update_score_header, score, id, Cid);
                if(mysql_query(&sql_connection, update_score))
                {
                    fprintf(stderr, "%s\n", mysql_error(&sql_connection));
                    printf("成绩修改失败\n");
                }
                else
                    printf("成绩修改成功\n");
            }
            else //delete score
            {
                char *delete_score_header = "DELETE FROM grade WHERE 学号 = ";
                char delete_score[strlen(delete_score_header)+31];
                sprintf(delete_score, "%s'%s' AND Cid = '%s'",
                        delete_score_header, id, Cid);
                printf("确定删除学号为%s学生的%s成绩？(y/n): ", id, row[0]);
                char answer = getchar();
                clean_input_stream();
                if(toupper(answer) == 'Y')
                {
                    if(mysql_query(&sql_connection, delete_score))
                    {
                        fprintf(stderr, "%s\n", mysql_error(&sql_connection));
                        printf("成绩删除失败\n");
                    }
                    else
                        printf("成绩删除成功\n");
                }
                else
                    fprintf(stderr, "取消删除成绩\n");
            }
        }
        mysql_free_result(result);
    }
}

int student_exists(const char* id)
{
    //the sql_select_header's length is 38(excluding '\0')
    char sql_select_statement[38+12] = "SELECT * FROM student WHERE 学号 = '";
    strcat(sql_select_statement, id);
    strcat(sql_select_statement, "'");
    if(mysql_query(&sql_connection, sql_select_statement))
    {
        fprintf(stderr, "%s\n", mysql_error(&sql_connection));
        return 0;
    }
    else
    {
        MYSQL_RES *result = mysql_store_result(&sql_connection);
        if(!result)
        {
            fprintf(stderr,"%s\n", mysql_error(&sql_connection));
            return 0;
        }
        //if the result is empty
        else if(!mysql_num_rows(result))
            return 0;
        else
            return 1;
    }
}
