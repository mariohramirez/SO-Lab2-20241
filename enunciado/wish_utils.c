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
        write(STDERR_FILENO, error_message2, strlen(error_message2));
        return;
    }

    if (strtok_r(NULL, " ", &newpath) != NULL)
    {
        write(STDERR_FILENO, error_message2, strlen(error_message2));
        return;
    }

    if (access(path, F_OK) == 0)
    {
        chdir(path);
    }
    else
    {
        write(STDERR_FILENO, error_message2, strlen(error_message2));
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
        i = 0;
        while (strcmp((*mypath)[i], "") != 0)
        {
            (*mypath)[i] = strdup("");
            i++;
        }    
    }
    else
    {
        *mypath = realloc(*mypath, (path_count + 1) * sizeof(char *));

        path = strtok_r(newpath, " ", &newpath);
        i = 0;
        while (path != NULL)
        {
            if (strcmp(path, "bin") == 0 || strcmp(path, "/bin") == 0 || strcmp(path, "/bin/") == 0 || strcmp(path, "./bin") == 0 || strcmp(path, "./bin/") == 0)
            {
                (*mypath)[i] = strdup("/bin/");
            }
            else
            {
                if (strstr(path, "./") != path)
                {
                    char *new_path = malloc(strlen(path) + 3);
                    strcpy(new_path, "./");
                    strcat(new_path, path);
                    (*mypath)[i] = new_path;
                }
                else
                {
                    (*mypath)[i] = strdup(path);
                }
                if (path[strlen(path) - 1] != '/')
                {
                    strcat((*mypath)[i], "/");
                }
            }
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

        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);

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
