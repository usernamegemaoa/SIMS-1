#include "project.h"

static char id[ID_LEN + 1];

int student_exists(const char* id);
static void add_grade();
static void change_or_query_grade();

void manage_grade()
{
    while(1)
    {
        int option = -1; //initialize with a invalid value
        printf("    + + + + + + + + + + + + + + + + + +\n"
               "    +    欢迎进入学生成绩管理界面     +\n"
               "    +                                 +\n"
               "    +        1):查询学生成绩          +\n"
               "    +        2):添加学生成绩          +\n"
               "    +        3):修改学生成绩          +\n"
               "    +        4):删除学生成绩          +\n"
               "    +        5):返回上一级            +\n"
               "    + + + + + + + + + + + + + + + + + +\n");
        printf("请选择合适的选项(输入相应的数字): ");
        scanf("%d", &option);
        clean_input_stream();
        switch(option)
        {
            case 1:
            case 2:
            case 3:
            case 4:
                printf("请输入学生的学号: ");
                Fgets_stdin(id, 11);
                if(!student_exists(id))
                    fprintf(stderr, "没有学号为%s的学生\n", id);
                else if(option == 2)
                    add_grade();
                else
                    change_or_query_grade(option);
                break;
            case 5:
                return;
            default:
                printf("输入有误，请重新输入\n");
                break;
        }
    }
}

static void add_grade()
{
    char *select_header =
        "SELECT Cname, Cid FROM course WHERE Cid not in"
        "(SELECT Cid FROM grade WHERE 学号 = ";
    char *select_statement = (char *)Malloc(strlen(select_header)
            + ID_LEN + 4); //the length of "'')\0" is 4
    sprintf(select_statement, "%s'%s')", select_header, id);

    if(mysql_query(mysql, select_statement))
        fprintf(stderr, "添加学生信息失败\n%s\n", mysql_error(mysql));
    else
    {
        MYSQL_RES *result = mysql_store_result(mysql);
        if(!result)
            fprintf(stderr, "%s\n", mysql_error(mysql));
        else if(!mysql_num_rows(result))
            fprintf(stderr, "该学生所有的课程都已经有成绩了，不能再添加成绩\n");
        else
        {
            MYSQL_ROW row = mysql_fetch_row(result);
            my_ulonglong num_rows = mysql_num_rows(result);
            int course_choice = 0;
            printf("该学生可以添加成绩的课程为:\n");
            if(num_rows == 1)
                printf("    %s\n", row[0]);
            else
            {
                for(unsigned int i = 0; i < num_rows; ++i)
                {
                    printf("    %d. %s\n", (i + 1), row[0]);
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
                mysql_data_seek(result, course_choice - 1);
                row = mysql_fetch_row(result);
            }
            int grade = -1;
            printf("请输入学生的%s成绩: ", row[0]);
            while(1)
            {
                scanf("%d", &grade);
                clean_input_stream();
                if(grade < 0 || grade > 100)
                    printf("成绩输入超出范围(0-100)，请重新输入: ");
                else
                    break;
            }

            char *sql_insert_header = "INSERT INTO grade VALUES(";
            char insert_grade[strlen(sql_insert_header)+10+4+5];
            char *course_id = row[1];
            sprintf(insert_grade, "%s'%s','%s','%d')",
                    sql_insert_header, id, course_id, grade);
            if(mysql_query(mysql, insert_grade))
                fprintf(stderr, "%s\n", mysql_error(mysql));
            else
                printf("成功添加学生成绩\n\n");
        }
        mysql_free_result(result);
    }
    free(select_statement);
}

/*
 * update or delete or query student's grade
 */
static void change_or_query_grade(int option)
{
    char *option_str[4] = {"查询", NULL, "修改", "删除"};
    char *select_header =
        "SELECT Cname, course.Cid, grade FROM course, grade WHERE"
        " course.Cid = grade.Cid"
        " AND grade.学号= ";
    char *select_statement = (char *)Malloc(strlen(select_header)
            + ID_LEN + 3); // the length of "''\0" is 3
    sprintf(select_statement, "%s'%s'", select_header, id);

    if(mysql_query(mysql, select_statement))
        fprintf(stderr, "%s失败\n%s\n",
                option_str[option - 1], mysql_error(mysql));
    else
    {
        MYSQL_RES *result = mysql_store_result(mysql);
        if(!result)
            fprintf(stderr, "%s\n", mysql_error(mysql));
        else if(!mysql_num_rows(result))
            fprintf(stderr, "该学生所有的课都没有成绩，无法进行%s\n",
                    option_str[option - 1]);
        else
        {
            MYSQL_ROW row = mysql_fetch_row(result);
            my_ulonglong num_rows = mysql_num_rows(result);
            int course_choice = 0;
            printf("该学生有成绩的课程为:\n");
            if(num_rows == 1)
                printf("    %s %s分\n", row[0], row[2]);
            else
            {
                for(unsigned int i = 0; i < num_rows; ++i)
                {
                    printf("    %d. %s %s分\n", (i + 1), row[0], row[2]);
                    row = mysql_fetch_row(result);
                }
            }
            printf("\n");
            if(option == 1) //query grade it's done
                return;
            else //update or delete grade
            {
                if(num_rows > 1) {
                    printf("请选择要%s成绩的课程(输入相应的数字): ",
                            option_str[option - 1]);
                    while(1)
                    {
                        scanf("%d", &course_choice);
                        clean_input_stream();
                        if(course_choice < 1 || course_choice > num_rows)
                            printf("输入有误，请重新输入: ");
                        else
                            break;
                    }
                    //seek to the choosen row
                    mysql_data_seek(result, course_choice - 1);
                    row = mysql_fetch_row(result);
                }
                char *Cid = row[1];
                //update grade
                if(option == 3)
                {
                    int grade = -1;
                    printf("请输入学生的%s成绩: ", row[0]);
                    while(1)
                    {
                        scanf("%d", &grade);
                        clean_input_stream();
                        if(grade < 0 || grade > 100)
                            printf("成绩输入超出范围(0-100)，请重新输入: ");
                        else
                            break;
                    }
                    char *update_grade_header = "UPDATE grade SET grade = ";
                    char *update_grade_stmt = (char *)Malloc(
                            strlen(update_grade_header)
                            + 2 + GRADE_LEN
                            + strlen(" WHERE 学号 = ''") + ID_LEN
                            + strlen(" AND Cid = ''") + COURSE_ID_LEN
                            + 1
                            );
                    sprintf(update_grade_stmt,
                            "%s'%d' WHERE 学号 = '%s' AND Cid = '%s'",
                            update_grade_header, grade, id, Cid);
                    if(mysql_query(mysql, update_grade_stmt))
                    {
                        fprintf(stderr, "%s\n", mysql_error(mysql));
                        printf("成绩修改失败\n");
                    }
                    else
                        printf("成绩修改成功\n");
                    free(update_grade_stmt);
                }
                else //delete grade
                {
                    char *delete_grade_header = "DELETE FROM grade WHERE 学号 = ";
                    char *delete_grade_stmt = (char *)Malloc(
                            strlen(delete_grade_header)
                            + 2 + COURSE_ID_LEN
                            + strlen(" AND Cid = ''") + COURSE_ID_LEN
                            + 1
                            );
                    sprintf(delete_grade_stmt, "%s'%s' AND Cid = '%s'",
                            delete_grade_header, id, Cid);
                    printf("确定删除学号为%s学生的%s成绩？(y/n): ", id, row[0]);
                    if(is_no())
                        fprintf(stderr, "取消删除成绩\n");
                    else if(mysql_query(mysql, delete_grade_stmt))
                            fprintf(stderr, "成绩删除失败\n%s\n",
                                    mysql_error(mysql));
                    else
                        printf("成绩删除成功\n");
                    free(delete_grade_stmt);
                }
            }
        }
        mysql_free_result(result);
    }
    free(select_statement);
}

int student_exists(const char* id)
{
    char *select_header = "SELECT * FROM student WHERE 学号 = ";
    char *select_statement = (char *)Malloc(
            strlen(select_header) + ID_LEN + 3);
    sprintf(select_statement, "%s'%s'", select_header, id);
    if(mysql_query(mysql, select_statement))
    {
        fprintf(stderr, "获取学生信息失败\n%s\n", mysql_error(mysql));
        return 0;
    }
    else
    {
        MYSQL_RES *result = mysql_store_result(mysql);
        if(!result)
        {
            fprintf(stderr,"获取学生信息失败%s\n", mysql_error(mysql));
            return 0;
        }
        //if the result is empty
        else if(!mysql_num_rows(result))
            return 0;
        else
            return 1;
    }
    free(select_statement);
}
