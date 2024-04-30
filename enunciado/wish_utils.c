#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "wish_utils.h"

char error_message2[30] = "An error has occurred\n";

void execute_exit(char *args){
    char *path = strtok_r(args, " ", &args);
    if(path != NULL){
        write(STDERR_FILENO, error_message2, strlen(error_message2)); 
    }else{
        exit(0);
    }
}

void execute_cd(char *newpath)
{
    char *path = strtok_r(newpath, " ", &newpath);
    if (path == NULL)
    {
        printf("Error: el comando cd requiere un argumento\n");
    }
    else
    {
        if (strtok_r(NULL, " ", &newpath) != NULL)
        {
            printf("Error: el comando cd solo acepta un argumento\n");
        }
        else
        {
            if (access(path, F_OK) == 0)
            {
                chdir(path);
                printf("La nueva ruta es %s\n", path);
            }
            else
            {
                printf("Directory does not exist: %s\n", path);
            }
        }
    }
}

void execute_path(char *newpath, char ***mypath)
{
    char *copypath = strdup(newpath);
    int path_count = 0;
    int i = 0;
    char *path = strtok_r(copypath, " ", &copypath);

    while (path != NULL)
    {
        path_count++;
        path = strtok_r(copypath, " ", &copypath);
    }

    if (path_count == 0)
    {
        printf("Variable mypath sin modificaciones\n");
    }
    else
    {
        *mypath = realloc(*mypath, (path_count + 1) * sizeof(char *));

        path = strtok_r(newpath, " ", &newpath);
        i = 0;
        while (path != NULL)
        {
            (*mypath)[i] = strdup(path);
            path = strtok_r(newpath, " ", &newpath);
            i++;
        }
        (*mypath)[i] = "";
    }
}

int wish_launch_redirect(char **args, char *file)
{
    pid_t pid, wpid;
    int status;
    pid = fork();
    if (pid == 0)
    {
        int fd = open(file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

        dup2(fd, 1);
        dup2(fd, 2);
        close(fd);
        execv(args[0], args);
    }
    else if (pid < 0)
    {
        write(STDERR_FILENO, error_message2, strlen(error_message2));
    }
    else
    {
        wait(NULL);
    }
    return 1;
}
