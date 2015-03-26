#ifndef C_EXPERIMENT
#define C_EXPERIMENT 1

#include <stdio.h>
#include <mysql.h>
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
