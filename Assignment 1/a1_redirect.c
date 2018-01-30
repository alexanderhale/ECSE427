#include<stdio.h>
#include<unistd.h>
int main() {   
    // print to stdout normally
    printf("First : Print to stdout\n");

    // switch to redirect_out.txt
    int redOut = dup(fileno(stdout));
    FILE *f;
    f = fopen("redirect_out.txt", "w");
    
    // print to redirected stdout
    fprintf(f, "Second : Print to redirect_out.txt\n");

    // switch back to stdout
    dup2(redOut,fileno(stdout));
    close(redOut);

    // print to stdout normally
    printf("Third : Print to stdout\n");
    return 0;
}