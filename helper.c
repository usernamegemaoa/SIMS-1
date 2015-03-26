#include <stdio.h>
#include <string.h>

void clean_input_stream()
{
    char c;
    while((c = getchar()) != '\n' && c != EOF)
        ; /*discard the input buffer*/
}

void fgets_remove_newline(char *s, int size, FILE *stream)
{
    if(!fgets(s, size, stream))
        fprintf(stderr, "Get user input failed\n");
    size_t length = strlen(s);
    /*
     * when the last character is '\n' which means the input buffer is clean
     * otherwise clear the buffer.
     */
    if(s[length - 1] == '\n')
        s[length - 1] = 0;
    else
        clean_input_stream();
}
