#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "wish_utils.h"
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_COMMANDS 1000
#define MAX_SIZE 500
#define BUFFER_SIZE 1024
#define HISTORY_SIZE 30

char error_message[30] = "An error has occurred\n";
char history[HISTORY_SIZE][BUFFER_SIZE];
int history_count = 0;

void procesar_comando(char *command, char ***mypath)
{
    int num_com = 0;
    int fd;

    if ((strstr(command, "cd") != 0 || strstr(command, "path") != 0 || strstr(command, "exit") != 0) && strstr(command, "&") == 0)
    {
        char *s = command;
        char *command_string = strtok_r(s, " ", &s);

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
            execute_path(s, mypath);
        }
    }
    else if (strstr(command, "cd") == NULL && strstr(command, "path") == NULL && strstr(command, "exit") == NULL)
    {
        num_com = 0;
        char *command_copy = strdup(command);
        char *token = strtok(command_copy, "&");
        while (token != NULL)
        {
            token += strspn(token, " ");
            if (*token != '\0')
            {
                num_com++;
            }
            token = strtok(NULL, "&");
        }

        char *all_commands[num_com + 1];
        char *command_copy2 = strdup(command);
        token = strtok(command_copy2, "&");
        int i = 0;
        while (token != NULL)
        {
            token += strspn(token, " ");
            if (*token != '\0')
            {
                all_commands[i] = token;
                i++;
            }
            token = strtok(NULL, "&");
        }
        all_commands[i] = NULL;

        pid_t pids[num_com];

        for (i = 0; i < num_com; i++)
        {
            char *s = all_commands[i];
            char *command_string = strtok_r(s, " ", &s);

            fd = -1;
            char ***mp_copy = malloc(2 * sizeof(char **));
            memcpy(mp_copy, mypath, 2 * sizeof(char **));
            char ***mp = mp_copy;
            char specificpath[MAX_SIZE];

            while ((strcmp((*mp)[0], "") != 0) && fd != 0)
            {
                strcpy(specificpath, *(mp[0]++));
                strncat(specificpath, command_string, strlen(command_string));
                fd = access(specificpath, X_OK);
            }

            if (fd == 0)
            {
                if ((pids[i] = fork()) < 0)
                {
                    perror("fork");
                    abort();
                }
                else if (pids[i] == 0)
                {
                    int num_args = 0;
                    char *s_copy = strdup(s);
                    char *token = strtok(s_copy, " ");
                    while (token != NULL)
                    {
                        num_args++;
                        token = strtok(NULL, " ");
                    }

                    i = 1;
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

                    int i_found = 0, aux = 0;
                    i = 0;
                    do
                    {
                        if (strcmp(myargs[i], ">") == 0)
                        {
                            aux = aux + 1;
                            i_found = i;
                        }
                        else if (strchr(myargs[i], '>') != NULL)
                        {
                            aux = aux + 2;
                        }

                        i++;
                    } while (myargs[i] != NULL);

                    if (aux == 1)
                    {
                        if (myargs[i_found + 1] == NULL)
                        {
                            write(STDERR_FILENO, error_message, strlen(error_message));
                        }
                        else if (myargs[i_found + 2] != NULL)
                        {
                            write(STDERR_FILENO, error_message, strlen(error_message));
                        }
                        else
                        {
                            char *file = malloc(MAX_SIZE * sizeof(char *));
                            strcpy(file, myargs[i_found + 1]);
                            myargs[i_found] = NULL;
                            myargs[i_found + 1] = NULL;
                             if (file != NULL)
                            {
                                myargs[0] = strdup(specificpath);
                                wish_launch_redirect(myargs, file);
                            }
                            else
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                            }
                        }
                    }
                    else if (aux > 1)
                    {
                        write(STDERR_FILENO, error_message, strlen(error_message));
                    }
                    else
                    {
                        execv(specificpath, myargs);
                    }
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
            }
        }

        int status;
        pid_t pid;
        for (i = 0; i < num_com; i++)
        {
            pid = wait(&status);
        }
    }
    else
    {
        write(STDERR_FILENO, error_message, strlen(error_message));
    }
}

int main(int argc, char *argv[])
{
    char **mypath = malloc(2 * sizeof(char *));
    mypath[0] = "/bin/";
    mypath[1] = "";

    char *input_line;

    if (argc == 1)
    {
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

            input_line[strcspn(input_line, "\n")] = '\0';
            procesar_comando(input_line, &mypath);

        } while (1);
        free(input_line);
    }
    else if (argc == 2)
    {
        char commands[MAX_COMMANDS][MAX_SIZE];
        int num_commands = 0;

        FILE *fp = fopen(argv[1], "r");
        if (!fp)
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
            return EXIT_FAILURE;
        }

        while (fgets(commands[num_commands], MAX_SIZE, fp))
        {
            num_commands++;
        }
        fclose(fp);

        for (int i = 0; i < num_commands; i++)
        {
            input_line = commands[i];
            input_line[strcspn(input_line, "\n")] = '\0';
            procesar_comando(input_line, &mypath);
        }
    }
    else if (argc > 2)
    {
        write(STDERR_FILENO, error_message, strlen(error_message));
        return EXIT_FAILURE;
    }

    return 0;
}
