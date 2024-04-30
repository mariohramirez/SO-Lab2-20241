#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "wish_utils.h"
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_SIZE 500
#define BUFFER_SIZE 1024
#define HISTORY_SIZE 30

char history[HISTORY_SIZE][BUFFER_SIZE];
int history_count = 0;

int main(int argc, char *argv[])
{
    char str[MAX_SIZE];
    char *command_string;
    char *s;
    int fd;
    char specificpath[MAX_SIZE];
    char *input_line;

    char **mypath = malloc(2 * sizeof(char *));
    mypath[0] = "/bin/";
    mypath[1] = "";

    do
    {
        input_line = readline("wish> ");
        if (!input_line)
        {
            break;
        }
        if (strlen(input_line) > 0)
        {
            add_history(input_line);

            if (history_count < HISTORY_SIZE)
            {
                strcpy(history[history_count++], input_line);
            }
            else
            {
                for (int i = 0; i < HISTORY_SIZE - 1; i++)
                {
                    strcpy(history[i], history[i + 1]);
                }
                strcpy(history[HISTORY_SIZE - 1], input_line);
            }
        }

        s = input_line;
        command_string = strtok_r(s, " ", &s);

        int i = 0;
        while (strcmp(mypath[i], "") != 0)
        {
            i++;
        }

        if (strcmp(command_string, "exit") == 0)
        {
            execute_exit(s);
        }
        else if (strcmp(command_string, "cd") == 0)
        {
            execute_cd(s);
        }
        else if (strcmp(command_string, "path") == 0)
        {
            execute_path(s, &mypath);
        }
        else
        {
            fd = -1;
            char **mp = mypath;
            char specificpath[MAX_SIZE];

            while ((strcmp(*mp, "") != 0) && fd != 0)
            {
                strcpy(specificpath, *mp++);
                strncat(specificpath, command_string, strlen(command_string));
                fd = access(specificpath, X_OK);
            }

            if (fd == 0)
            {
                int subprocess = fork();

                if (subprocess < 0)
                {
                    printf("Error launching the subprocess");
                }
                else if (subprocess == 0)
                {
                    int num_args = 0;
                    char *s_copy = strdup(s);
                    char *token = strtok(s_copy, " ");
                    while (token != NULL)
                    {
                        num_args++;
                        token = strtok(NULL, " ");
                    }

                    int i = 1;
                    char *myargs[num_args + 1];
                    myargs[0] = strdup(command_string);

                    char *s_copy2 = strdup(s);
                    token = strtok(s_copy2, " ");
                    while (token != NULL)
                    {
                        myargs[i] = token;
                        token = strtok(NULL, " ");
                        i++;
                    }
                    myargs[i] = NULL;

                    execv(specificpath, myargs);
                }
                else
                {
                    wait(NULL);
                }
            }
            else
            {
                printf("Command not found: %s\n", str);
            }
        }
        free(input_line);
    } while (1);
    return 0;
}
