#ifndef C_EXPERIMENT
#define C_EXPERIMENT 1

#include <stdio.h>
#include <mysql.h>

//数据库中各表每列定义的长度
#define ID_LEN 10
#define STU_NAME_LEN 16
#define SEX_LEN 3
#define CLASS_LEN 3
#define SPECIALITY_LEN 30
#define TEL_NUM_LEN 11
#define ADDRESS_LEN 60
#define COURSE_ID_LEN 4
#define COURSE_NAME_LEN 30
#define COURSE_CREDIT_LEN 4
#define MAX_FIELD_LEN ADDRESS_LEN

//defined in main.c
extern MYSQL sql_connection;
/*
 * defined in helper.c
 */
extern void clean_input_stream();
/*
 * defined in helper.c
 * do like fgets, but remove potential newline
 * and clear input stream
 */
extern void fgets_remove_newline(char *s, int size, FILE *stream);
#endif
