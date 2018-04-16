#include <stdio.h>
#include <stdlib.h>

int i = 0;  // number of processes
int j = 0;  // number of resources
int *avail;    // array of available resources
int **max;      // 2D array of the max number of each resource that a process could need
int **need;     // 2D array of the amount of each resource that a process still needs
int **hold;     // 2D array of the amount of each resource that a process is currently holding

// implementation of isSafe() as described in the slides
int isSafe(int j, int i) {
    // initialize and fill temporary arrays (isSafe step 1)
    int *work = malloc(j * sizeof(int));
    int *finish = malloc(i * sizeof(int));  
    int t, s;
    for (t = 0; t < j; t++) {
        work[t] = avail[t];
    }
    for (t = 0; t < i; t++) {
        finish[t] = 0;
    }

    // isSafe algorithm step 2-3
    for (t = 0; t < i; t++) {
        if (finish[t] == 0) {
            for (s = 0; s < j; s++) {
                if (need[t][s] <= work[s]) {        // process i's needs are <= the available resources
                    work[s] = work[s] + hold[t][s]; // simulate process i releasing what it was holding, because it has finished
                    finish[t] = 1;
                }
            }
        }
    }

    free(work);

    // isSafe step 4
    for (t = 0; t < i; t++) {
        // if any process has not progressed, return false; otherwise return true
        if (finish[t] == 0) {
            free(finish);
            return 0;
        }
    }
    free(finish);
    return 1;
}

// implementation of Banker's Algorithm as described in the slides
    // returns 1 if the request is granted, 0 if it is not granted
int bankers_algorithm(int pr_id, int req) {
    int s;
    for (s = 0; s < j; s++) {
        if req[s] > need[pr_id][s]) {
            // return an error, a process can't request more than it needs
            return -1;
        } else if req[s] > avail[s]) {
            // the process needs more resources than are available, so it must wait.
            return 0;
        } else {
            // provisional allocation of this resource
            avail[s] = avail[s] - req[s];
            hold[pr_id][s] = hold[pr_id][s] + req[s];
            need[pr_id][s] = need[pr_id][s] - req[s];

            if (!isSafe(j, i)) {
                // the system isn't safe with this resource allocation, so revert the changes made
                avail[s] = avail[s] + req[s];
                hold[pr_id][s] = hold[pr_id][s] - req[s];
                need[pr_id][s] = need[pr_id][s] + req[s];
            }
        }
    }
    // no issues were found with the resource allocation requested, so return a success
    return 1;
}

// simulates processes running on the system
void* process_simulator(void* pr_id) {
    int pid = *(int *)pr_id;
    int moreRequests = 1;

    // this process continues executing while there are more requests
    while(moreRequests) {
        int *req = malloc(j * sizeof(int));
        int s;
        printf("Requesting resources for process %d\n", pid);
        char *requestVector = malloc(2 * j * sizeof(char));
        for (s = 0; s < j; s++) {
            req[s] = rand() % need[pid][s];      // random resource request vector generated (step 2)
            requestVector += (char)req[s];
            requestVector += ' ';
        }
        printf("Resource request vector: %s\n", requestVector);
        free(requestVector);

        printf("Checking if request can be allocated\n");
        while (!bankers_algorithm(pid, req)) {       // run Banker's algorithm to check if we can allocate the requested resources (step 3)
            // busy wait by iterating until Banker's deems that the requested resources can be allocated (step 6)
        }
        free(req);
        printf("Request is allowed: allocating\n");
        // there is no print for a rejected allocation, because Banker's is called in the while loop above and it would spam the feed

        // the process has acquired the resources, it can keep executing (step 4)
        moreRequests = 0;
        for (s = 0; s < j; s++) {
            if (need[pid][s] > 0) {
                moreRequests = 1;       // if this process has more requests to request, continue executing (step 5b)
            }
        }
        if (moreRequests) {
            usleep(3*1000000);          // sleep the thread to simulate the process's continued execution (step 5b)
        }
    }
    return NULL;        // if this process doesn't have any more resources to request, it can terminate (step 5a)
}

// simulates a fault occuring on the system.
void* fault_simulator(void* pr_id) {
    while(1) {
        int resourceToBeDoomed = rand() % j
        int couldBeDoomed = rand() % 10;

        if (couldBeDoomed > 5) {
            avail[resourceToBeDoomed]--;        // take away one unit of the randomly selected resource
            usleep(10*1000000);
            avail[resourceToBeDoomed]++;        // fault is over, return the resource
        } else {
            usleep(10*1000000);                 // no doom applied this time, take a nap and we'll try again in 10 seconds
        }       
    }
}

// checks for deadlock
void* deadlock_checker(void * pr_id) {
    int t, s;
    int deadlock;
    while(!deadlock) {
        deadlock = 1;               // begin by assuming we have deadlock
        for (t = 0; t < i; t++) {
            for (s = 0; s < j; s++) {
                if (need[t][s] < avail[s]) {
                    deadlock = 0;   // if any process can acquire any amount of any resource, then we don't have deadlock anymore
                }
            }
        }

        if (deadlock) {
            printf("Deadlock will occur as processes request more resources, exiting.\n");
        }
        
        sleep(10*1000000);   // take a nap for 10 seconds until we want to check for deadlock again
        printf("Checking for deadlock\n");
    }
    return NULL;
}

int main (int argc, char *argv[]) {
    int s, t;   // counters for loops
    
    //Initialize all inputs to banker's algorithm based on the inputs
    printf("Number of processes: ");
    scanf("%d", &i);        // number of processes

    printf("\nNumber of resources: ");
    scanf("%d", &j);        // number of resources
    
    avail = malloc(j * sizeof(int));    // units of available resources
    max = (int**)malloc(i * sizeof(int *));      // max units of each resource that a process could need
    need = (int**)malloc(i * sizeof(int *));     // units of each resource that a process still needs
    hold = (int**)malloc(i * sizeof(int *));     // units of each resource that a process is currently holding
    for (s = 0; s < i; s++) {
        max[s] = (int*)malloc(j * sizeof(int));
        need[s] = (int*)malloc(j * sizeof(int));
        hold[s] = (int*)malloc(j * sizeof(int));
    }

    printf("\nAvailable resources (in order, hit ENTER between each): ");
    for (s = 0; s < j; s++) {
        scanf("%d", &avail[s]);     // number of units of each resource  
    }

    printf("\nMax resource claim of each process (in order, hit ENTER between each): ");
    for (t = 0; t < i; t++) {
        for (s = 0; s < j; s++) {
            scanf("%d", &max[t][s]);        // max number of units each process can claim
            hold[t][s] = 0;     // resource units that each process currently holds (starts at 0)
            need[t][s] = max[t][s];     // resource units that each process still needs (starts at max)
        }
    }

    char *resourceNumbers = malloc(2 * j * sizeof(char));
    for (s = 0; s < j; s++) {
        resourceNumbers += (char)(avail[s]);        // this number also needs to include hold[i][s] for all i, but those are all zero at this point
        resourceNumbers += ' ';
    }
    printf("\n\nThe number of each resource in the system is: %s", resourceNumbers);
    free(resourceNumbers);

    char *allocatedResources = malloc((i * j + (i * (j+1)) + i) * sizeof(char));
    for (t = 0; t < i; t++) {
        for (s = 0; s < j; s++) {
            allocatedResources += (char)hold[t][s];
            allocatedResources += ' ';
        }
        allocatedResources += "| ";
    }
    printf("\n\nThe number of each type of resources that each process is holding is: %s", allocatedResources);
    free(allocatedResources);

    char *maximumResources = malloc((i * j + (i * (j+1)) + i) * sizeof(char));
    for (t = 0; t < i; t++) {
        for (s = 0; s < j; s++) {
            maximumResources += (char)max[t][s];
            maximumResources += ' ';
        }
        maximumResources += "| ";
    }
    printf("\n\nThe maximum number of each type of resources that each process can claim is: %s", maximumResources);
    free(maximumResources);

    char *availableResources = malloc(2 * j * sizeof(char));
    for (s = 0; s < j; s++) {
        availableResources += (char)avail[s];
        availableResources += ' ';
    }
    printf("\n\nThe available units of each resource are: %s", availableResources);
    free(availableResources);

    //create threads simulating processes (process_simulator)
    pthread_t processThreads[i];
    int *process_ids[i];

    for (s = 0; s < i; s++) {
        process_ids[s] = malloc(sizeof(int));
        *process_ids[s] = s;
        if (pthread_create(&processThreads[s], NULL, process_simulator, process_ids[s]) != 0) {
            printf("Error with process %d's thread creation.", s);
        }
    }

    // create a thread that takes away resources from the available pool (fault_simulator)
    pthread_t fault_thread;
    int *fault_id = malloc(sizeof(int));
    *fault_id = i+2;
    if (pthread_create(&fault_thread, NULL, fault_simulator, fault_id) != 0) {
        printf("Error with fault simulator thread creation.")
    }


    //create a thread to check for deadlock (deadlock_checker)
    pthread_t deadlock_thread;
    int *deadlock_id = malloc(sizeof(int));
    *deadlock_id = i+1;
    if (pthread_create(&deadlock_thread, NULL, deadlock_checker, deadlock_id) != 0) {
        printf("Error with deadlock checker thread creation.");
    }

    pthread_exit(NULL);
    return 0;
}