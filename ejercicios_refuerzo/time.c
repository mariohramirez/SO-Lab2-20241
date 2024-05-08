#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>


int main(int argc, char const *argv[])
{
    struct timeval tI;
    struct timeval tF;
    pid_t hijo;
    long elapsedTime;
    

    gettimeofday(&tI, NULL);
    printf("%d\n", tI.tv_usec);
    printf("%d\n", argc);

    int total = 0;
    for (int i = 1; i < argc; ++i) {
        total += strlen(argv[i]) + 1; 
    }

    char *command = malloc(total);
    command[0] = '\0';

    for (int i = 1; i < argc; ++i) {
        strcat(command, argv[i]);
        if (i < argc - 1) {
            strcat(command, " ");
        }
    }

    hijo = fork();
    if(hijo == 0){
        execlp("sh", "sh", "-c", command, (char *) NULL);
    } else {
        wait(NULL);
        gettimeofday(&tF, NULL);
        printf("%d\n", tF.tv_usec);
        elapsedTime = tF.tv_usec - tI.tv_usec;
        printf("Elapsed time: %ld", elapsedTime);
    }
    
    free(command);
    
    return 0;
}