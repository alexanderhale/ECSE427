/*
----------------- COMP 310/ECSE 427 Winter 2018 -----------------
I declare that the awesomeness below is a genuine piece of work
and falls under the McGill code of conduct, to the best of my knowledge.
-----------------------------------------------------------------
*/ 

//Please enter your name and McGill ID below
//Name: Alexander Hale
//McGill ID: 260672475

//all the header files you will require
#include <stdio.h>  //for standard IO
#include <unistd.h> //for execvp/dup/dup2
#include <string.h> //for string manipulation
#include <stdlib.h> //for fork  
#include <ctype.h>  //for character type check (isnum,isalpha,isupper etc)
#include <sys/wait.h>//for waitpid
#include <fcntl.h>  //open function to open a file. type "man 2 open" in terminal
#include <time.h>   //to handle time

//pointer to Linked list head
struct node *head_job = NULL;

//pointer to current spot in Linked list
struct node *current_job = NULL;

//global variable used to store process id of process that has been just created
//can be used while adding a job to linked list
pid_t process_id;

//flag variable to check if redirection of output is required
int isred = 0;

//structure of a single node
//do not modify this structure
struct node
{
    int number;        //the job number
    int pid;           //the process id of the process
    char *cmd;         //string to store the command name
    time_t spawn;      //time to store the time it was spawned
    struct node *next; //when another process is called you add to the end of the linked list
};

// Add a job to the linked list
void addToJobList(char *args[])
{
    //allocate memory for the new job
    struct node *job = malloc(sizeof(struct node));
    //If the job list is empty, create a new head
    if (head_job == NULL)
    {
        //init the job number with 1
        job->number = 1;

        //set its pid from the global variable process_id
        job->pid = process_id;

        //cmd can be set to args[0]
        job->cmd = &args[0];				// TODO: unsure

        //set the job->next to point to NULL.
        job->next = NULL;

        //set the job->spawn using time function
        job->spawn = (unsigned int)time(NULL);

        //set head_job to be the job
        head_job = job;

        //set current_job to be head_job
        current_job = head_job;
    }

    //Otherwise create a new job node and link the current node to it
    else
    {
        //point current_job to head_job, then traverse the list to find the last item in the list
        current_job = head_job;
        while (current_job->next != NULL) {
        	current_job = current_job->next;
        }
        // c now points to the last job in the list 

        //init all values of the job
        job->number = c->number + 1;
        job->pid = process_id;
        job->cmd = &args[0];		// TODO: unsure
        job->next = NULL;			// indicate that this job is the last in the list by setting it's next to NULL
        job->spawn = (unsigned int)time(NULL);
        
        //make next of current_job point to job
        current_job->next = job;

        //make job to be current_job
        current_job = job;        
    }
}

// Function to refresh job list:
// Run through jobs in linked list and check if they are done executing. If not, remove it
void refreshJobList()
{
    //pointer require to perform operation 
    //on linked list
    struct node *current_job;
    struct node *prev_job;
    
    //variable to store returned pid 
    pid_t ret_pid;

    //perform init for pointers
    current_job = head_job;
    prev_job = head_job;

    //traverse through the linked list
    while (current_job != NULL) {
        //use waitpid to init ret_pid variable
        ret_pid = waitpid(current_job->pid, NULL, WNOHANG);
        //one of the below needs node removal from linked list
        if (ret_pid == 0)
        {
            // if we're here, it means that the waited-for process has not terminated
            	// therefore, we don't need to do anything to it: leave it alone
        	// update our place in the list (move forward one item)
        	prev_job = current_job;
        	current_job = current_job->next;
        } else {
            // if we're here, it means that the waited-for process has terminated
            	// therefore, we should remove it to clear the memory space
        	prev_job->next = current_job->next;	// mark the previous job's next as the current job's next, removing the current job from the list
        	free(current_job);					// remove the current job from memory
            
            // update our place in the list (move forward one item)
        	current_job = prev_job->next;
            prev_job = current_job;
            current_job = current_job->next;
        }
    }
    return;
}

//Function that list all the jobs
void listAllJobs()
{
    struct node *current_job;
    int ret_pid;

    //refresh the linked list
    refreshJobList();

    //init current_job with head_job
    current_job = head_job;

    //heading row print only once.
    printf("\nID\tPID\tCmd\tstatus\tspawn-time\n");
        
    //traverse the linked list 
	while (current_job != NULL) {
        // print using the following statement for each job
    	printf("%d\t%d\t%s\tRUNNING\t%s\n", current_job->number, current_job->pid, current_job->cmd, ctime(&(current_job->spawn)));

		current_job = current_job->next;	// move to the next item in the linked list
    }

           
        
    return;
}

// wait till the linked list is empty
// TODO: look for a place where you would call this function.
// do not modify this function
void waitForEmptyLL(int nice, int bg)
{
    if (nice == 1 && bg == 0)
    {
        while (head_job != NULL)
        {
            sleep(1);
            refreshJobList();
        }
    }
    return;
}

//function to perform word count
 int wordCount(char *filename, char* flag)
 {
     FILE *fp = fopen(filename,"r");
     if (fp == NULL) {
        printf("Could not open file %s", filename);
        return 0;
     }
     int cnt;
     char c;
     if (*flag == 'l') {
     	// Extract characters from file and store in character c. Continue until c is the end of the file
    	for (c = getc(fp); c != EOF; c = getc(fp))
        	if (c == '\n')
            	cnt++;		// add one for each new line
     } else if (*flag == 'w') {
     	// Extract characters from file and store in character c. Continue until c is the end of the file
    	for (c = getc(fp); c != EOF; c = getc(fp))
        	if (c == '\n' || c == ' ')
            	cnt++;		// add one for each new line and for each space
     } else {
     	// something is wrong, return nonsensical value
     	cnt = -1;
     }
     fclose(fp);	// close the file
     return cnt;
 }

// function to augment waiting times for a process
// do not modify this function
void performAugmentedWait() {
    int w, rem;
    time_t now;
    srand((unsigned int)(time(&now)));
    w = rand() % 15;
    printf("sleeping for %d\n", w);
    rem = sleep(w);
    return;
}

//simulates running process to foreground by making the parent process wait for a particular process id.
int waitforjob(char *jobnc) {
    struct node *trv;
    int jobn = (*jobnc) - '0';
    trv = head_job;

    // traverse through the linked list
    while (trv != NULL) {
    	// if the job with the reqested PID is found
    	if (trv->pid == jobn) {
    		while (waitpid(current_job->pid, NULL, WUNTRACED) != trv->pid) {
    			// while trv is still running, this loop will continue
    			// when trv finishes, this will return trv->pid, so we can continue
    			// TODO: make sure this logic is correct
    		}
    		trv = NULL;	// explicitly set TRV, because we've found the correct job, dealt with it, and we can leave now
    	} else {
    	   	trv = trv->next;	// move to next item in linkedlist
	    }
    }
    
    return 0;
}

// splits whatever the user enters and sets the background/nice flag variable
// and returns the number of tokens processed
// do not modify this function
int getcmd(char *prompt, char *args[], int *background, int *nice)
{
    int length, i = 0;
    char *token, *loc;
    char *line = NULL;
    size_t linecap = 0;
    printf("%s", prompt);
    length = getline(&line, &linecap, stdin);
    if (length <= 0)
    {
        exit(-1);
    }
    // Check if background is specified..
    if ((loc = index(line, '&')) != NULL)
    {
        *background = 1;
        *loc = ' ';
    }
    else
        *background = 0;
    while ((token = strsep(&line, " \t\n")) != NULL)
    {
        for (int j = 0; j < strlen(token); j++)
            if (token[j] <= 32)
                token[j] = '\0';
        if (strlen(token) > 0)
        {
            if (!strcmp("nice", token))
            {
                *nice = 1;
            }
            else
            {
                args[i++] = token;
            }
        }
    }
    return i;
}

// this initialises the args to All null.
// do not modify
void initialize(char *args[])
{
    for (int i = 0; i < 20; i++)
    {
        args[i] = NULL;
    }
    return;
}

int main(void)
{
    //args is a array of charater pointers
    //where each pointer points to a string
    //which may be command , flag or filename
    char *args[20];

    //flag variables for background, status and nice
    //bg set to 1 if the command is to executed in background
    //nice set to 1 if the command is nice
    //status  
    int bg, status, nice;

    //variable to store the process id.
    pid_t pid;

    // variable to store the file destination
    // helpful in output redirection
    int fd1, fd2;

    //your terminal executes endlessly unless 
    //exit command is received
    while (1)
    {
        //init background to zero
        bg = 0;
        //init nice to zero
        nice = 0;
        //init args to null
        initialize(args);
        //get the user input command
        int cnt = getcmd("\n>> ", args, &bg, &nice);
        //keep asking unless the user enters something
        while (!(cnt >= 1))
            cnt = getcmd("\n>> ", args, &bg, &nice);

        //use the if-else ladder to handle built-in commands
        //built in commands don't need redirection
        //also no need to add them to jobs linked list
        //as they always run in foreground
        if (!strcmp("jobs", args[0])) {
            //call the listalljobs function
            listAllJobs();
        } else if (!strcmp("exit", args[0])) {
            //exit the execution of endless while loop 
            exit(0);
        } else if (!strcmp("fg", args[0])) {
            //bring a background process to foreground
            waitforjob(args[1]);
        } else if (!strcmp("cd", args[0])) {
            int result = 0;
            if (args[1] == '') {
            	args[1] = '/';			// if no destination directory given, change to the home directory
            } else {
            	if (chdir(args[1]) < 0) {
            		// given directory doesn't exist or caused some other error, print an error message
            		printf("Given directory doesn't exist, or another error occured.\n");
            		// TODO: be more specific about this error message
            	}
            	// otherwise, we've successfully changed to the destination directory
            }
        } else if (!strcmp("pwd", args[0])) {
            int size = 20			// TODO: how large should I make this buffer?
            char *buf[size];
            printf(getcwd(buf, size));    //use getcwd and print the current working directory
        } else if(!strcmp("wc",args[0])) {
            //call the word count function
            wordCount(args[2],args[1]);
        } else {
            //Now handle the executable commands here 
            /* the steps can be..:
            (1) fork a child process using fork()
            (2) the child process will invoke execvp()
            (3) if background is not specified, the parent will wait,
                otherwise parent starts the next command... */


            //hint : samosas are nice but often there is a long waiting line for it.

            //create a child
            pid = fork();

            //to check if it is parent
            if (pid > 0) {
                //we are inside parent
                //printf("inside parent\n");
                if (bg == 0) {
                    //FOREGROUND
                    // waitpid with proper argument required
                    waitpid(pid, NULL, WUNTRACED);		// foreground process: must wait for child process to terminate
                } else {
                    //BACKGROUND
                    process_id = pid;
                    addToJobList(args);					// add the new child process to the linked list
                    waitpid(pid, NULL, WNOHANG);		// background process: no need to wait for child process to terminate
                }
            } else {
                // we are inside the child
            	// printf("inside child\n");

                //introducing augmented delay
                performAugmentedWait();

                // check for redirection: scan args looking for '>'
                int isred = 0;
                int length = sizeof(a)/sizeof(a[0]);			// length of args list
                for (int i = 1; i < length; i++) {
                	if (args[i] == '>') {
                		isred = i;		// we have a redirection. Use isred to store the location of the >
                		i = length;		// don't need to check rest of list, we can exit immediately
                	}
                }

                //if redirection is enabled
                if (isred >= 1)
                {
                    // TODO
                    //open file and change output from stdout to that  
                    //make sure you use all the read write exec authorisation flags
                    //while you use open (man 2 open) to open file

                    //set ">" and redirected filename to NULL
                    args[isred] = NULL;
                    args[i + 1] = NULL;

                    //run your command
                    execvp(args[0], args);

                    //restore to stdout
                    fflush(stdout);
                }
                else
                {
                    //simply execute the command.
                    execvp(args[0], args);
                }
            }
        }
    }

    return 0;
}