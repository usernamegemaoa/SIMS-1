#include "project.h"

static void add_information();
static void update_information();
static void delete_information();

void manage_information()
{
    while(1)
    {
        int option = -1; //initialize with a invalid value
        printf("    + + + + + + + + + + + + + + + + + +\n"
               "    +   欢迎进入学生基本信息管理界面  +\n"
               "    +                                 +\n"
               "    +          1):添加学生信息        +\n"
               "    +          2):修改学生信息        +\n"
               "    +          3):删除学生信息        +\n"
               "    +          4):返回上一级          +\n"
               "    + + + + + + + + + + + + + + + + + +\n");
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
    char id[ID_LEN + 1];
    char name[STU_NAME_LEN + 1];
    char sex[SEX_LEN + 1];
    char speciality[SPECIALITY_LEN + 1];
    char class[CLASS_LEN + 1];
    char tel_number[TEL_NUM_LEN + 1];
    char address[ADDRESS_LEN + 1];
    char *sql_insert_header = "INSERT INTO student VALUES(";

    while(1)
    {
        printf("请输入学生信息\n学号: ");
        while(1)
        {
            Fgets_stdin(id, sizeof(id));
            if(id[0] && strlen(id) == ID_LEN)
            {
                char *invalid_char = NULL;
                //test whether or not the input is valid
                //if it is, then *invalid_char equals to null character
                strtol(id, &invalid_char, 10);
                if(*invalid_char == '\0')
                    break;
            }
            printf("学号不能为空,且必须是10个数字,请重新输入: ");
        }
        printf("姓名: ");
        while(1)
        {
            Fgets_stdin(name, sizeof(name));
            if(name[0])
                break;
            printf("姓名不能为空,请重新输入: ");
        }
        printf("性别(男/女): ");
        while(1)
        {
            Fgets_stdin(sex, sizeof(sex));
            if(!strcmp(sex, "男") || !strcmp(sex, "女"))
                break;
            printf("性别输入有误,请重新输入: ");
        }
        printf("注意: 输入以下信息时直接回车,表示暂不填写相应信息\n");
        printf("专业: ");
        Fgets_stdin(speciality, sizeof(speciality));
        printf("班级: ");
        Fgets_stdin(class, sizeof(class));
        printf("联系电话: ");
        Fgets_stdin(tel_number, sizeof(tel_number));
        printf("籍贯: ");
        Fgets_stdin(address, sizeof(address));

        //INSERT INTO student VALUES('id','name','sex','class',
        //'speciality','tel_number','address')
        char *insert_statement = (char *)Malloc(
                strlen(sql_insert_header)
                + ID_LEN + 3 //the length is strlen("'id',")
                + STU_NAME_LEN + 3
                + SEX_LEN + 3
                + (speciality[0] ?
                    strlen(speciality) + 3 : NULL_LEN + 1)
                + (class[0] ?
                    strlen(class) + 3 : NULL_LEN + 1)
                + (tel_number[0] ?
                    TEL_NUM_LEN + 3: NULL_LEN + 1)
                + (address[0] ?
                    strlen(address) + 3 : NULL_LEN + 1)
                + 1);
        sprintf(insert_statement,
                "%s'%s','%s','%s',", sql_insert_header, id, name, sex);
        if(speciality[0])
            sprintf(insert_statement + strlen(insert_statement),
                    "'%s‘,", speciality);
        else
            strcat(insert_statement, "NULL,");
        if(class[0])
            sprintf(insert_statement + strlen(insert_statement),
                    "'%s‘,", class);
        else
            strcat(insert_statement, "NULL,");
        if(tel_number[0])
            sprintf(insert_statement + strlen(insert_statement),
                    "'%s‘,", tel_number);
        else
            strcat(insert_statement, "NULL,");
        if(address[0])
            sprintf(insert_statement + strlen(insert_statement),
                    "'%s‘,",address);
        else
            strcat(insert_statement, "NULL,");
        //change trailing comma to parentheses
        insert_statement[strlen(insert_statement) - 1] = ')';
        if(mysql_query(mysql, insert_statement))
            fprintf(stderr,"添加失败\n%s\n", mysql_error(mysql));
        else
            printf("成功添加学生信息\n");
        free(insert_statement);
        printf("继续添加其他学生信息?(y/n): ");
        if(is_no())
            break;
    }
}

static void update_information()
{
    char *sql_select_header = "SELECT * FROM student WHERE 学号 = ";
    while(1)
    {
        char id[ID_LEN + 1];
        printf("请输入要修改信息的学生的学号: ");
        while(1)
        {
            Fgets_stdin(id, sizeof(id));
            if(id[0] && strlen(id) == ID_LEN)
            {
                char *invalid_char = NULL;
                //test whether or not the input is valid
                //if it is, then *invalid_char equals to null character
                strtol(id, &invalid_char, 10);
                if(*invalid_char == '\0')
                    break;
            }
            printf("学号不能为空,且必须是10个数字,请重新输入: ");
        }
        char *select_statement = (char *)Malloc(
                strlen(sql_select_header) + ID_LEN + 2 + 1);
        sprintf(select_statement, "%s'%s'", sql_select_header, id);
        if(mysql_query(mysql, select_statement))
            fprintf(stderr, "查询失败\n%s\n", mysql_error(mysql));
        else
        {
            MYSQL_RES *result = mysql_store_result(mysql);
            if(!result)
                fprintf(stderr,"查询失败\n%s\n", mysql_error(mysql));
            //if the result is empty
            else if(!mysql_num_rows(result))
                fprintf(stderr, "没有学号为%s的学生\n", id);
            else
            {
                char name[STU_NAME_LEN + 1] = "";
                char sex[SEX_LEN + 1] = "";
                char speciality[SPECIALITY_LEN + 1] = "";
                char class[CLASS_LEN + 1] = "";
                char tel_number[TEL_NUM_LEN + 1] = "";
                char address[ADDRESS_LEN + 1] = "";
                //new_info[1]不使用
                char *new_info[STU_NUM_FIELDS] = { NULL, name, sex,
                    speciality, class, tel_number, address};
                int new_info_fields_lens[STU_NUM_FIELDS] = { 0, sizeof(name),
                    sizeof(sex), sizeof(speciality), sizeof(class),
                    sizeof(tel_number), sizeof(address) };
                MYSQL_ROW row = mysql_fetch_row(result);
                MYSQL_FIELD *fields = mysql_fetch_fields(result);
                printf("对于每一项信息，如果要修改则输入新的信息，否则直接回车跳过相应的项\n");
                for(unsigned int i = 1; i < STU_NUM_FIELDS; ++i)
                {
                    printf("%s[%s]: ", fields[i].name, row[i] ? row[i] : "NULL");
                    Fgets_stdin(new_info[i], new_info_fields_lens[i]);
                }

                char *update_header = "UPDATE student set ";
                char *update_statement = (char *)Malloc(
                        strlen("UPDATE student SET 姓名='', 性别='', "
                            "专业='', 班级='' 联系电话='', 籍贯='' ")
                        + strlen(name) + SEX_LEN + strlen(speciality)
                        + CLASS_LEN + TEL_NUM_LEN + strlen(address) + 1
                        );
                sprintf(update_statement, "%s", update_header);
                int need_update = 0;
                for(unsigned int i = 1; i < STU_NUM_FIELDS; ++i)
                {
                    if(new_info[i][0] != '\0')
                    {
                        need_update = 1;
                        char name_value_pair[fields[i].name_length
                            + new_info_fields_lens[i] + 4];
                        sprintf(name_value_pair, "%s='%s',",
                                fields[i].name, new_info[i]);
                        strcat(update_statement, name_value_pair);
                    }
                }

                if(need_update)
                {
                    //change the trailing comma to whitespace
                    update_statement[strlen(update_statement) - 1] = ' ';
                    char update_statement_tailer[
                            strlen("WHERE 学号 = ''") + ID_LEN + 1];
                    sprintf(update_statement_tailer, "WHERE 学号 = '%s'", id);
                    strcat(update_statement,update_statement_tailer);
                    if(mysql_query(mysql, update_statement))
                        fprintf(stderr,"更新失败\n%s\n", mysql_error(mysql));
                    else
                        printf("更新成功\n");
                }
                else
                    printf("没有修改信息，无须更新.\n");
                free(update_statement);
            }
            mysql_free_result(result);
        }
        free(select_statement);

        printf("继续修改其他学生信息?(y/n): ");
        if(is_no())
            break;
    }
}

static void delete_information()
{
    while(1)
    {
        char id[ID_LEN + 1];
        char *delete_header = "DELETE FROM student WHERE 学号 = ";
        printf("请输入要删除学生的学号: ");
        Fgets_stdin(id, sizeof(id));
        char *delete_statement = (char *)Malloc(strlen(delete_header)
                + ID_LEN + 2 + 1);
        sprintf(delete_statement, "%s'%s'", delete_header, id);
        if(mysql_query(mysql, delete_statement))
            fprintf(stderr,"删除失败\n%s\n", mysql_error(mysql));
        else if(!mysql_affected_rows(mysql))
            fprintf(stderr, "没有学号为%s的学生\n", id);
        else
            printf("删除成功.\n");
        free(delete_statement);

        printf("继续删除其他学生信息?(y/n): ");
        if(is_no())
            break;
    }
}
