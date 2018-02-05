/*
----------------- COMP 310/ECSE 427 Winter 2018 -----------------
I declare that the awesomeness below is a genuine piece of work
and falls under the McGill code of conduct, to the best of my knowledge.
-----------------------------------------------------------------
*/ 

//Please enter your name and McGill ID below
//Name: Alexander Hale
//McGill ID: 260672475

// include the packages we need
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include <sys/wait.h>//for waitpid

int main() {

    int f[2];               // array for file descriptor pair
    char readbuffer[512*sizeof(char)];   // buffer to store what's going to be read later
    
    // start the pipe
    if (pipe(f) < 0) {
        printf("Fatal error\n");
        return 0;
    }

    int pid = fork();
    if (pid == 0) {
        // Child: execute ls using execvp

        char* arg[2] = {"ls", NULL};                   //  array to hold the ls command
        close(f[0]);                                   // child closes reading side of pipe
        dup2(f[1], 1);                                 // redirect stdout
        execvp(arg[0], arg);                           // execute the ls command
        exit(0);                                       // close the child process if execvp failed
    } else {
        // Parent: print output from ls here

        close(f[1]);                                   // parent closes writing side of pipe
        read(f[0], readbuffer, sizeof(readbuffer));    // read the pipe output
        close(f[0]);                                   // parent is done reading
        printf("Received output: %s\n", readbuffer);        // print the pipe output
        
        waitpid(pid, NULL, WUNTRACED);     // wait for child process to finish before continuing on
    }
    return 0;
}