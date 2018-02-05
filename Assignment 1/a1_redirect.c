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
#include <fcntl.h>
int main() {   
    // print to stdout normally
    printf("First : Print to stdout\n");

    // get file descriptor for new (or opened) file
    int filedesc = open("redirect_out.txt", O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
    
    // save file descriptor for stdout
    int saved_stdout = dup(1);
    
    // redirect the standard output to filedesc
    dup2(filedesc, 1);

    // print to the redirect file dump
    printf("Second : Print to redirect_out.txt\n");
    
    // restore stdout, then close the temporary holding variable
    dup2(saved_stdout, 1);
    close(saved_stdout);

    // print to stdout normally
    printf("Third : Print to stdout\n");
    return 0;
}