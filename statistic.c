#include "project.h"

static void query_grade();
static void query_info();

void statistic()
{
    while(1) {
        int option = -1; //initialize with a invalid value
        printf("    + + + + + + + + + + + + + + +\n"
               "    +    欢迎进入统计查询界面   +\n"
               "    +                           +\n"
               "    +      1):学生信息查询      +\n"
               "    +      2):成绩统计查询      +\n"
               "    +      3):返回上一级        +\n"
               "    + + + + + + + + + + + + + + +\n");
        printf("请选择合适的选项(输入相应的数字): ");
        scanf("%d", &option);
        clean_input_stream();
        switch(option) {
            case 1:
                query_info();
                break;
            case 2:
                query_grade();
                break;
            case 3:
                return;
            default:
                printf("输入有误，请重新输入\n");
                break;
        }
    }
}

static void query_info()
{
    char speciality[SPECIALITY_LEN + 1] = "";
    char class[CLASS_LEN + 1] = "";
    char stu_name[STU_NAME_LEN + 1] = "";
    char id[ID_LEN + 1] = "";
    char *select_student_header = "SELECT * FROM student";
    //存放select 语句的 where 子句
    char *where_clause[4] = {NULL};
    //where 子句的长度
    int  where_clause_len = 0;
    //判断是否有输入限制条件
    int  had_input = 0;
    printf("注意: 当要求输入某项限制条件时,"
            "直接回车表示不对此项进行限制!\n");
    printf("请输入学生所在专业: ");
    Fgets_stdin(speciality, sizeof(speciality));
    printf("请输入学生所在班级: ");
    Fgets_stdin(class, sizeof(class));
    printf("请输入学生姓名: ");
    Fgets_stdin(stu_name, sizeof(stu_name));
    printf("请输入学生学号: ");
    Fgets_stdin(id, sizeof(id));

    if(speciality[0]) {
        where_clause[0] = (char *)Malloc(strlen(" WHERE 专业=''")
                + strlen(speciality) + 1);
        sprintf(where_clause[0], " WHERE 专业='%s'", speciality);
        where_clause_len += strlen(where_clause[0]);
        had_input = 1;
    }
    if(class[0]) {
        where_clause[1] = (char *)Malloc(
                had_input ? strlen(" AND ") : strlen(" WHERE ")
                + strlen("班级=''")
                + strlen(class) + 1);
        sprintf(where_clause[1], "%s班级='%s'",
                had_input ? " AND " : " WHERE ",
                class);
        where_clause_len += strlen(where_clause[1]);
        had_input = 1;
    }
    if(stu_name[0]) {
        where_clause[2] = (char *)Malloc(
                had_input ? strlen(" AND ") : strlen(" WHERE ")
                + strlen("姓名=''")
                + strlen(stu_name) + 1);
        sprintf(where_clause[2], "%s姓名='%s'",
                had_input ? " AND " : " WHERE ",
                stu_name);
        where_clause_len += strlen(where_clause[2]);
        had_input = 1;
    }
    if(id[0]) {
        where_clause[3] = (char *)Malloc(
                had_input ? strlen(" AND ") : strlen(" WHERE ")
                + strlen(id) + 1);
        sprintf(where_clause[3], "%s学号='%s'",
                had_input ? " AND " : " WHERE ",
                id);
        where_clause_len += strlen(where_clause[3]);
        had_input = 1;
    }
    if(had_input == 0) {
        printf("没有输入任何限制条件,是否输出全部学生信息?(y/n): ");
        if(is_no()) {
            printf("取消查询\n");
            return;
        }
    }
    char *sql_select_student = (char *)Malloc(
            strlen(select_student_header)
            + where_clause_len
            + 1
            );
    sprintf(sql_select_student, "%s%s%s%s%s", select_student_header,
            where_clause[0] ? where_clause[0] : "",
            where_clause[1] ? where_clause[1] : "",
            where_clause[2] ? where_clause[2] : "",
            where_clause[3] ? where_clause[3] : "");
    if(mysql_query(mysql, sql_select_student)) {
        fprintf(stderr, "查询失败\n%s\n", mysql_error(mysql));
        return;
    }
    free(sql_select_student);
    if(had_input) {
        for(int i = 0; i < 4; ++i)
            if(where_clause[i])
                free(where_clause[i]);
    }
    MYSQL_RES *result = mysql_store_result(mysql);
    if(!result) {
        fprintf(stderr, "查询失败\n%s\n",
                mysql_error(mysql));
        return;
    }
    my_ulonglong num_rows = mysql_num_rows(result);
    if(num_rows == 0)
        fprintf(stderr, "没有找到符合条件的学生\n");
    else {
        MYSQL_FIELD *fields = mysql_fetch_fields(result);
        unsigned int num_fields = mysql_num_fields(result);
        //存放输出学生信息时表中每一项的宽度
        int print_width[num_rows][num_fields];
        set_print_width(result, num_rows + 1, num_fields,
                print_width);
        printf("总共找到%llu名符合条件的学生\n", num_rows);
        //print table header
        for(int i = 0; i < num_fields; i++)
            printf("| %-*s ", print_width[0][i], fields[i].name);
        printf("|\n");
        MYSQL_ROW row;
        for(int i = 1; i < num_rows + 1; i++) {
            row = mysql_fetch_row(result);
            for(int j = 0; j < num_fields; j++)
                printf("| %*s ", print_width[i][j], row[j] ? row[j] : "NULL");
            printf("|\n");
        }
    }
}

static void query_grade()
{
    while(1) {
        int option = -1; //initialize with a invalid value
        printf("    + + + + + + + + + + + + + + + +\n"
               "    +       欢迎进入成绩查询界面  +\n"
               "    +                             +\n"
               "    +      1):班级总成绩排名查询  +\n"
               "    +      2):课程成绩排名查询    +\n"
               "    +      3):返回上一级          +\n"
               "    + + + + + + + + + + + + + + + +\n");
        printf("请选择合适的选项(输入相应的数字): ");
        scanf("%d", &option);
        clean_input_stream();
        switch(option) {
            case 1:
                {
                    char speciality[SPECIALITY_LEN + 1];
                    char class[CLASS_LEN + 1];
                    char *select_grade_header =
                        "select student.学号, 姓名, sum(grade) as 总分 "
                        "from student, grade "
                        "where student.学号 = grade.学号 ";
                    char *speciality_pair = NULL; // " and 专业 = 'speciality'"
                    char *class_pair = NULL; // " and 班级 = 'class'"
                    char *select_grade_tail =
                        "group by 学号 order by 总分 desc ";
                    //" limit row_count_str"
                    printf("请输入专业名: ");
                    while(1) {
                        Fgets_stdin(speciality, sizeof(speciality));
                        if(speciality[0]) {
                            char *pair = (char *)Malloc(strlen("and 专业 ='' ")
                                    + strlen(speciality) + 1);
                            sprintf(pair, "and 专业 = '%s' ", speciality);
                            speciality_pair = pair;
                            break;
                        } else
                            printf("专业名不能为空，请重新输入: ");
                    }
                    printf("请输入班级名: ");
                    while(1) {
                        Fgets_stdin(class, sizeof(class));
                        if(class[0]) {
                            char *pair = (char *)Malloc(strlen("and 班级 = '' ")
                                    + strlen(class) + 1);
                            sprintf(pair, "and 班级 = '%s' ", class);
                            class_pair = pair;
                            break;
                        } else
                            printf("班级名不能为空，请重新输入: ");
                    }
                    printf("注意:直接回车表示输出全部学生成绩\n"
                            "请问要输出前多少名学生的成绩: ");
                    char row_count_str[10] = "";
                    int row_count = 0;
                    char *invalid_char = NULL;
                    while(1) {
                        Fgets_stdin(row_count_str,
                                sizeof(row_count_str));
                        if(row_count_str[0] == 0) //output all students
                            break;
                        row_count = strtol(row_count_str, &invalid_char, 10);
                        if(row_count > 0) {
                            //truncating the invalid string
                            *invalid_char = '\0';
                            break;
                        }
                        printf("输入有误，请重新输入: \n");
                    }
                    char *sql_select_grade = (char*)Malloc(
                            strlen(select_grade_header)
                            + strlen(speciality_pair)
                            + strlen(class_pair)
                            + strlen(select_grade_tail)
                            + (row_count ?
                                strlen("limit ") + strlen(row_count_str) : 0)
                            + 1
                            );
                    sprintf(sql_select_grade, "%s%s%s%s",
                            select_grade_header,
                            speciality_pair,
                            class_pair,
                            select_grade_tail);
                    free(speciality_pair);
                    free(class_pair);
                    if(row_count) {
                        strcat(sql_select_grade, "limit ");
                        strcat(sql_select_grade, row_count_str);
                    }
                    if(mysql_query(mysql, sql_select_grade)) {
                        fprintf(stderr, "查询失败\n%s\n",
                                mysql_error(mysql));
                        exit(1);
                    }
                    free(sql_select_grade);
                    MYSQL_RES *result = mysql_store_result(mysql);
                    if(!result) {
                        fprintf(stderr, "查询失败\n%s\n",
                                mysql_error(mysql));
                        break;
                    }
                    my_ulonglong num_rows = mysql_num_rows(result);
                    if(num_rows == 0)
                        fprintf(stderr, "没有该班的学生成绩信息\n");
                    else {
                        MYSQL_FIELD *fields = mysql_fetch_fields(result);
                        unsigned int num_fields = mysql_num_fields(result);
                        //存放输出成绩信息时表中每一项的宽度
                        int print_width[num_rows + 1][num_fields];
                        set_print_width(result, num_rows + 1, num_fields,
                                print_width);
                        //print table header
                        printf("| 排名 ");
                        for(int i = 0; i < num_fields; i++)
                            printf("| %-*s ", print_width[0][i], fields[i].name);
                        printf("|\n");
                        //print table body
                        MYSQL_ROW row;
                        for(int i = 1; i < num_rows + 1; i++) {
                            row = mysql_fetch_row(result);
                            //print rank number
                            printf("| %4d ", i);
                            for(int j = 0; j < num_fields; j++) {
                                printf("| %*s ", print_width[i][j], row[j] ? row[j] : "NULL");
                            }
                            printf("|\n");
                        }
                    }
                }
                break;
            case 2:
                {
                    char course[COURSE_NAME_LEN + 1];
                    char *select_grade_header =
                        "select student.学号, 姓名, grade as 分数 "
                        "from student, grade, course "
                        "where student.学号=grade.学号 "
                        "and grade.Cid = course.Cid "
                        "and Cname = ";
                    char *select_grade_tail = " order by 分数 desc ";
                    //" limit row_count_str"
                    printf("请输入课程名: ");
                    while(1) {
                        Fgets_stdin(course, sizeof(course));
                        if(course[0]) //had input course name
                            break;
                        printf("课程名不能为空，请重新输入: ");
                    }
                    printf("注意:直接回车表示输出全部学生成绩\n"
                            "请问要输出前多少名学生的成绩: ");
                    char row_count_str[10] = "";
                    int row_count = 0;
                    char *invalid_char = NULL;
                    while(1) {
                        Fgets_stdin(row_count_str,
                                sizeof(row_count_str));
                        if(row_count_str[0] == 0) //output all students
                            break;
                        row_count = strtol(row_count_str, &invalid_char, 10);
                        if(row_count > 0) {
                            //truncating the invalid string
                            *invalid_char = '\0';
                            break;
                        }
                        printf("输入有误，请重新输入: \n");
                    }
                    char *sql_select_grade = (char*)Malloc(
                            strlen(select_grade_header)
                            + strlen(course) + 2 //要算上单引号
                            + strlen(select_grade_tail)
                            + (row_count ?
                              strlen("limit ") + strlen(row_count_str) : 0)
                            + 1);
                    sprintf(sql_select_grade, "%s'%s'%s",
                            select_grade_header,
                            course,
                            select_grade_tail);
                    if(row_count) {
                        strcat(sql_select_grade, "limit ");
                        strcat(sql_select_grade, row_count_str);
                    }
                    if(mysql_query(mysql, sql_select_grade)) {
                        fprintf(stderr, "查询失败\n%s\n",
                                mysql_error(mysql));
                        break;
                    }
                    free(sql_select_grade);
                    MYSQL_RES *result = mysql_store_result(mysql);
                    if(!result) {
                        fprintf(stderr, "查询失败\n%s\n",
                                mysql_error(mysql));
                        break;
                    }
                    my_ulonglong num_rows = mysql_num_rows(result);
                    if(num_rows == 0)
                        fprintf(stderr, "没有该课程的学生成绩信息\n");
                    else {
                        MYSQL_FIELD *fields = mysql_fetch_fields(result);
                        unsigned int num_fields = mysql_num_fields(result);
                        //存放输出成绩信息时表中每一项的宽度
                        int print_width[num_rows + 1][num_fields];
                        set_print_width(result, num_rows + 1, num_fields,
                                print_width);
                        //print table header
                        printf("| 排名 ");
                        for(int i = 0; i < num_fields; i++)
                            printf("| %-*s ", print_width[0][i], fields[i].name);
                        printf("|\n");
                        //print table body
                        MYSQL_ROW row;
                        for(int i = 1; i < num_rows + 1; i++) {
                            row = mysql_fetch_row(result);
                            //print rank number
                            printf("| %4d ", i);
                            for(int j = 0; j < num_fields; j++) {
                                printf("| %*s ", print_width[i][j], row[j] ? row[j] : "NULL");
                            }
                            printf("|\n");
                        }
                    }
                }
                break;
            case 3:
                return;
            default:
                printf("输入有误, 请重新输入\n");
                break;
        }
    }
}
