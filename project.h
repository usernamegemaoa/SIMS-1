#ifndef C_EXPERIMENT
#define C_EXPERIMENT 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <mysql.h>

// the name of database
#define DATABASE "Cexp"
// the length of database user name
#define USER_NAME_LEN 40
// the length of database user password
#define PASSWD_LEN 20
//数据库中各表每列定义的长度
#define STU_NUM_FIELDS 7
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
#define GRADE_LEN 3
// #define MAX_FIELD_LEN ADDRESS_LEN
//NULL_LEN is strlen("NULL")
#define NULL_LEN 4

//defined in main.c
extern MYSQL *mysql;
extern char current_user[];
extern char passwd[];
/*
 * defined in helper.c
 */
extern void clean_input_stream();
/*
 * defined in helper.c
 * read input from stdin like fgets
 * but remove potential newline
 * and clear input stream
 */
extern void Fgets_stdin(char *s, int size);

extern void set_print_width(MYSQL_RES *result, int rows, int cols,
        int print_width[rows][cols]);
extern void *Malloc(size_t size);
extern int is_no(void);
#endif
