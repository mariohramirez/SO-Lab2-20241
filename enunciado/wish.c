#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE 1024
#define MAX_ARGS 128
#define WHITESPACE " \t\n"

char error_message[30] = "An error has occurred\n";

void showError() {
    write(STDERR_FILENO, error_message, strlen(error_message));
}

void processCmd(char *cmd, char **path);

void runCmd(char **args, char **path, int background);

int isBuiltInCmd(char **args);

void executeBuiltInCmd(char **args, char **path);

void changeDir(char **args);

void setPath(char **args, char **path);

int main(int argc, char *argv[]) {
    char *cmd = NULL;
    size_t len = 0;
    FILE *input_stream = stdin;

    if (argc == 2) {
        input_stream = fopen(argv[1], "r");
        if (!input_stream) {
            showError();
            exit(1);
        }
    } else if (argc > 2) {
        showError();
        exit(1);
    }

    char *path[MAX_ARGS] = {"/bin", NULL};

    while (1) {
        if (argc == 1) {
            printf("wish> ");
        }
        if (getline(&cmd, &len, input_stream) == -1) {
            if (argc == 2) fclose(input_stream);
            exit(0);
        }

        processCmd(cmd, path);
        free(cmd);
        cmd = NULL;
    }
}

void processCmd(char *cmd, char **path) {
    char *args[MAX_ARGS];
    int argc = 0;
    int background = 0;
    char *token = strtok(cmd, WHITESPACE);

    while (token != NULL) {
        if (strcmp(token, "&") == 0) {
            background = 1;
        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, WHITESPACE);
            if (!token || strtok(NULL, WHITESPACE)) {
                showError();
                return;
            }
            FILE *output_file = fopen(token, "w");
            if (!output_file) {
                showError();
                return;
            }
            dup2(fileno(output_file), STDOUT_FILENO);
            fclose(output_file);
        } else {
            args[argc++] = token;
        }
        token = strtok(NULL, WHITESPACE);
    }
    args[argc] = NULL;

    if (args[0] == NULL) return;

    if (!isBuiltInCmd(args)) {
        runCmd(args, path, background);
    } else {
        executeBuiltInCmd(args, path);
    }
}


void runCmd(char **args, char **path, int background) {
    int status;
    pid_t pid = fork();
    if (pid == 0) {
        char execpath[MAX_LINE];
        snprintf(execpath, sizeof(execpath), "%s/%s", path[0], args[0]);
        execv(execpath, args);
        showError();
        exit(1);
    } else if (pid > 0) {
        if (!background) {
            waitpid(pid, &status, 0);
        }
    } else {
        showError();
    }
}




int isBuiltInCmd(char **args) {
    if (strcmp(args[0], "exit") == 0 ||
        strcmp(args[0], "cd") == 0 ||
        strcmp(args[0], "path") == 0) {
        return 1;
    }
    return 0;
}

void executeBuiltInCmd(char **args, char **path) {
    if (strcmp(args[0], "exit") == 0) {
        if (args[1] != NULL) {
            showError();
            return;
        }    
        exit(0);
    } else if (strcmp(args[0], "cd") == 0) {
        changeDir(args);
    } else if (strcmp(args[0], "path") == 0) {
        int i = 0;
        while (path[i] != NULL) {
            printf("%s\n", path[i]);
            i++;
        }
    }
}

void changeDir(char **args) {
    if (args[1] == NULL || args[2] != NULL) {
        showError();
    } else if (chdir(args[1]) != 0) {
        showError();
    }
}

void setPath(char **args, char **path) {
    int i = 1;
    int j = 0;
    while (args[i] != NULL) {
        path[j++] = args[i++];
    }
    path[j] = NULL;
}
