#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
int main() {

    int f[2];
    char readbuffer[100];
    pipe(f);

    if (fork() == 0) {
        // Child: execute ls using execvp
        char *arg[2];
        arg[0] = "ls";
        arg[1] = NULL;
        close(f[0]);    // child closes input side of pipe
        execvp(arg[0], arg);
        exit(0);
    } else {
        // Parent: print output from ls here
        close(f[1]);    // parent closes output side of pipe
        read(f[0], readbuffer, sizeof(readbuffer));
        printf("Received output:", readbuffer);
    }
    return 0;
}