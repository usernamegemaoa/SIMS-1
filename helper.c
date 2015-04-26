#include "project.h"

/*
 * clean input buffer
 */
void clean_input_stream()
{
    char c;
    while((c = getchar()) != '\n' && c != EOF)
        ; /*discard the input buffer*/
}

/*
 * do like fgets but remove trailing '\n'
 */
void Fgets_stdin(char *s, int size)
{
    if(!fgets(s, size, stdin)) {
        fprintf(stderr, "Get user input failed\n");
        exit(1);
    }
    size_t length = strlen(s);
    /*
     * when the last character is '\n' which means the input buffer is clean
     * otherwise clear the buffer.
     */
    if(s[length - 1] == '\n')
        s[length - 1] = '\0';
    else
        clean_input_stream();
}

void *Malloc(size_t size)
{
    void *result = malloc(size);
    if(result)
        return result;
    fprintf(stderr, "Malloc error\n");
    exit(1);
}

/*
 * return (true)1 if user input 'N'
 */
int is_no(void)
{
        char answer = getchar();
        clean_input_stream();
        return toupper(answer) == 'N';
}

/*
 * get the number of chinese word in character string s
 */
int get__num_chinese_words(char *s)
{
    int num = 0;
    if(s == NULL)
        return 0;
    //suppose chinses word is encoded by utf-8, three bytes
    //in almost all case, it's true
    for(int i = 0; s[i]; i++)
        if(s[i] < 0) {
            num++;
            i += 2;
        }
    return num;
}

/*
 * get the appropriate length modifier for printf() to print chinses
 * character string aligned
 */
void set_print_width(MYSQL_RES *result, int rows, int cols, int print_width[rows][cols])
{
    MYSQL_FIELD *fields = mysql_fetch_fields(result);
    //save the whole column's max print width
    int max_print_width[cols];
    //save the string length of fields
    int str_len[rows + 1][cols];
    //save the number of chinese word in each fields
    //the width of actual print is equal to str_len + num_chinese_word
    int num_chinese_word[rows + 1][cols];

    //check table fields name
    for(int i = 0; i < cols; i++) {
        num_chinese_word[0][i] = get__num_chinese_words(fields[i].name);
        str_len[0][i] = fields[i].name_length;
        //suppose the max print length is field print length
        max_print_width[i] = str_len[0][i] - num_chinese_word[0][i];
    }
    //check table body
    MYSQL_ROW row;
    for(int i = 1; i < rows; i++) {
        row = mysql_fetch_row(result);
        for(int j = 0; j < cols; j++) {
            num_chinese_word[i][j] = get__num_chinese_words(row[j]);
            if(row[j] == NULL)
                str_len[i][j] = NULL_LEN;
            else
                str_len[i][j] = strlen(row[j]);
            if(max_print_width[j] < str_len[i][j] - num_chinese_word[i][j])
                max_print_width[j] = str_len[i][j] - num_chinese_word[i][j];
        }
    }
    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
            print_width[i][j] = max_print_width[j] + num_chinese_word[i][j];
    mysql_data_seek(result, 0);
}
