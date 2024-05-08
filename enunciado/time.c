#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>


int main(int argc, char const *argv[])
{
    struct timeval tI;
    struct timeval tF;
    pid_t hijo;
    long elapsedTime;

    gettimeofday(&tI, NULL);
    printf("%d\n", tI.tv_usec);
    hijo = fork();
    if(hijo == 0){
        execlp("sh", "sh", "-c", argv[1], NULL);
        printf("PUnto");
    } else {
        wait(NULL);
        gettimeofday(&tF, NULL);
        printf("%d\n", tF.tv_usec);
        elapsedTime = tF.tv_usec - tI.tv_usec;
        printf("Elapsed time: %ld", elapsedTime);
    }
    
    
    return 0;
}