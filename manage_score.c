#include <stdio.h>
#include <string.h>
#include <mysql.h>
#include "project.h"

void manage_score()
{
    if(mysql_query(&sql_connection, "SELECT * FROM student WHERE 学号 = '1234567890'"))
    {
        printf("query error\n");
        exit(EXIT_FAILURE);
    }
    printf("select successfully\n");
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
