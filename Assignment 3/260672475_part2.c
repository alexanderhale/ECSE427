// NAME: Alex Hale
// ID: 260672475

#include <stdio.h>  //for printf and scanf
#include <stdlib.h> //for malloc

#define LOW 0
#define HIGH 199
#define START 53

//compare function for qsort
//you might have to sort the request array
//use the qsort function 
// an argument to qsort function is a function that compares 2 quantities
//use this there.
int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

//function to swap 2 integers
void swap(int *a, int *b) {
    if (*a != *b) {
        *a = (*a ^ *b);
        *b = (*a ^ *b);
        *a = (*a ^ *b);
    }
    return;
}

// function to find the indexes where START would fit in an array
    // the value returned is the index BELOW the index where START would go
        // i.e. the value at the returned index is the highest value in the array that is still lower than START
    // array must be sorted!
int indexOfStart(int *array, int arrayLength) {
    int i;
    int highestValue = array[0];
    int highestValueIndex = -1;
    for (i = 1; i < arrayLength; i++) {
        if (array[i] > highestValue && array[i] < START) {
            highestValue = array[i];
            highestValueIndex = i;
        }
    }

    return highestValueIndex;
}


//Prints the sequence and the performance metric
void printSeqNPerformance(int *request, int numRequest) {
    int i, last, acc = 0;
    last = START;
    printf("\n");
    printf("%d", START);
    for (i = 0; i < numRequest; i++)
    {
        printf(" -> %d", request[i]);
        acc += abs(last - request[i]);
        last = request[i];
    }
    printf("\nPerformance : %d\n", acc);
    return;
}

//access the disk location in FCFS
void accessFCFS(int *request, int numRequest) {
    // first come, first serve
    	// print in the order of arrival
    printf("\n----------------\n");
    printf("FCFS :");
    printSeqNPerformance(request, numRequest);
    printf("----------------\n");
    return;
}

//access the disk location in SSTF
	// shortest search time first
void accessSSTF(int *request, int numRequest) {
    // find the closest integer to START, put in request[0]
    // find the closest integer to request[0], put in request[1]
    // etc
    int i, j;
	int diff = HIGH - LOW;
	int index = -1;
	for (i = 0; i < numRequest; i++) {
		if (abs(START - request[i]) < diff) {
			diff = abs(START - request[i]);
			index = i;
		}
	}
	swap(&request[0], &request[index]);

	for (i = 1; i < numRequest - 1; i++) {
		diff = abs(request[i-1] - request[i]);
        index = i;
		for (j = i+1; j < numRequest; j++) {
			if (abs(request[i-1] - request[j]) < diff) {
				diff = abs(request[i-1] - request[j]);
				index = j;
			}
		}
        if (i != index) {
		    swap(&request[i], &request[index]);
        }
	}

    printf("\n----------------\n");
    printf("SSTF :");
    printSeqNPerformance(request, numRequest);
    printf("----------------\n");
    return;
}

//access the disk location in SCAN
void accessSCAN(int *request, int numRequest) {
    // increase array size by 1 to fit the boundary printout
    int newCnt = numRequest + 1;
    int *newRequest = malloc(newCnt * sizeof(int));

    // copy values into new array
    int i;
    for (i = 0; i < numRequest; i++) {
        newRequest[i] = request[i];
    }
    
    if (START < (HIGH-LOW)/2) {
        // move down first to reduce unproductive overlap
        // add LOW to the array so the lower boundary gets printed
        newRequest[newCnt - 1] = LOW;

        // sort the values
        qsort(newRequest, newCnt, sizeof(int), cmpfunc);

        // find the lowest value that is lower than START
        int indexBelow = indexOfStart(newRequest, newCnt);

        // exchange the values below START to be accessed in descending order
        i = 0;
        while (i < indexBelow/2) {
            swap(&newRequest[i], &newRequest[indexBelow - i]);
            i++;
        }

        // we're done! The values above START will be accessed in the proper ascending order
    } else {
        // move up first to reduce unproductive overlap
        // add HIGH to that array so the upper boundary gets printed
        newRequest[newCnt - 1] = HIGH;

        // sort the values
        qsort(newRequest, newCnt, sizeof(int), cmpfunc);

        // find the lowest value that is higher than START
        int indexAbove = indexOfStart(newRequest, newCnt) + 1;

        // move the values above START to be printed first, in ascending order
        for (i = indexAbove; i < newCnt; i++) {
            swap(&newRequest[i - indexAbove], &newRequest[i]);
        }

        // sort the values below START into ascending order
            // index of last-moved number is newCnt - indexAbove
        qsort(&newRequest[newCnt - indexAbove], indexAbove, sizeof(int), cmpfunc);

        // rearrange the values below START into descending order
        i = newCnt - indexAbove;
        int midway = i + (newCnt - 1 - i)/2;
        while (i < midway) {
            swap(&newRequest[i], &newRequest[midway + (midway - i)]);
            i++;
        }
    }
	
    printf("\n----------------\n");
    printf("SCAN :");
    printSeqNPerformance(newRequest, newCnt);
    printf("----------------\n");
    return;
}

//access the disk location in CSCAN
void accessCSCAN(int *request, int numRequest) {
    // SCANNING IN ASCENDING ORDER

    // sort array
    qsort(request, numRequest, sizeof(int), cmpfunc);

    // find where START fits in
    int indexBelow = indexOfStart(request, numRequest);

    int newCnt;
    int *newRequest;

    if (indexBelow < 0) {
        // if START fits in the first index, print the array in ascending order
        newCnt = numRequest;
        newRequest = malloc(newCnt * sizeof(int));

        // copy values into new array
        int i;
        for (i = 0; i < newCnt; i++) {
            newRequest[i] = request[i];
        }
    } else if (indexBelow == numRequest - 1) {
        // if START fits in the last index, print START -> HIGH -> LOW -> array in ascending order
        newCnt = numRequest + 2;
        newRequest = malloc(newCnt * sizeof(int));

        newRequest[0] = HIGH;
        newRequest[1] = LOW;

        int i;
        for (i = 0; i < numRequest; i++) {
            newRequest[i+2] = request[i];
        }
    } else  {
        // if START fits in the middle somewhere:
            // print START -> the values above START, in ascending order
            // print HIGH -> LOW
            // print the values below START, in ascending order
        newCnt = numRequest + 2;
        newRequest = malloc(newCnt * sizeof(int));

        int i;
        for (i = indexBelow + 1; i < numRequest; i++) {
            newRequest[i - indexBelow - 1] = request[i];
        }

        newRequest[i - indexBelow - 1] = HIGH;
        newRequest[i - indexBelow] = LOW;

        int j;
        for (j = 0; j <= indexBelow; j++) {
            newRequest[i - indexBelow + 1 + j] = request[j];
        }
    }

    printf("\n----------------\n");
    printf("CSCAN :");
    printSeqNPerformance(newRequest, newCnt);
    printf("----------------\n");
    return;
}

//access the disk location in LOOK
void accessLOOK(int *request, int numRequest) {
    // sort array
    qsort(request, numRequest, sizeof(int), cmpfunc);

    // find where START fits in
    int indexBelow = indexOfStart(request, numRequest);

    int newCnt = numRequest;
    int *newRequest = malloc(newCnt * sizeof(int));

     // if request[numRequest - 1] is closer to START
    if (abs(request[numRequest - 1] - START) < abs(request[0] - START)) {
        // print the values above START, in ascending order
        int i;
        for (i = indexBelow + 1; i < numRequest; i++) {
            newRequest[i - indexBelow - 1] = request[i];
        }

        // print the values below START, in descending order
        for (i = indexBelow; i >= 0; i--) {
            newRequest[numRequest - i - 1] = request[i];
        }
    } else {
        // request[0] is closer to START

        // print the values below START, in descending order
        int i;
        for (i = indexBelow; i >= 0; i--) {
            newRequest[indexBelow - i] = request[i];
        }

        // print the values above START, in ascending order
        for (i = indexBelow + 1; i < newCnt; i++) {
            newRequest[i] = request[i];
        }
    }
    
    printf("\n----------------\n");
    printf("LOOK :");
    printSeqNPerformance(newRequest, newCnt);
    printf("----------------\n");
    return;
}

//access the disk location in CLOOK
void accessCLOOK(int *request, int numRequest) {
    // LOOKING IN ASCENDING ORDER
        // NOTE: it was unclear from my notes whether the heads are 
        // returned to LOW or to the lowest request. I chose the lowest
        // request, hopefully that's correct.

    // sort array
    qsort(request, numRequest, sizeof(int), cmpfunc);

    // find where START fits in
    int indexBelow = indexOfStart(request, numRequest);

    int newCnt = numRequest;
    int *newRequest = malloc(newCnt * sizeof(int));
    if (indexBelow < 0) {
        // if START fits in the first index
        // print the values above START, in ascending order
        newCnt = numRequest;
        newRequest = malloc(newCnt * sizeof(int));

        // copy values into new array
        int i;
        for (i = 0; i < newCnt; i++) {
            newRequest[i] = request[i];
        }
    } else if (indexBelow == numRequest - 1) {
        // if START fits in the last index

        // print LOW
        // newRequest[i - indexBelow] = LOW;

        // print the values in ascending order
        int i;
        for (i = 1; i < newCnt; i++) {
            newRequest[i] = request[i-1];
        }
    } else {
        // if START fits in the middle somewhere

        // print the values above START, in ascending order
        int i;
        for (i = indexBelow + 1; i < numRequest; i++) {
            newRequest[i - indexBelow - 1] = request[i];
        }

        // print LOW
        // newRequest[i - indexBelow] = LOW;

        // print the values below START, in ascending order
        for (i = 0; i < indexBelow + 1; i++) {
            newRequest[numRequest - (indexBelow - i) - 1] = request[i];
        }
    }
    printf("\n----------------\n");
    printf("CLOOK :");
    printSeqNPerformance(newRequest,newCnt);
    printf("----------------\n");
    return;
}

int main()
{
    int *request, numRequest, i, ans;

    //allocate memory to store requests
    printf("Enter the number of disk access requests : ");
    scanf("%d", &numRequest);
    request = malloc(numRequest * sizeof(int));

    printf("Enter the requests ranging between %d and %d\n", LOW, HIGH);
    for (i = 0; i < numRequest; i++)
    {
        scanf("%d", &request[i]);
    }

    printf("\nSelect the policy : \n");
    printf("----------------\n");
    printf("1\t FCFS\n");
    printf("2\t SSTF\n");
    printf("3\t SCAN\n");
    printf("4\t CSCAN\n");
    printf("5\t LOOK\n");
    printf("6\t CLOOK\n");
    printf("----------------\n");
    scanf("%d",&ans);

    switch (ans)
    {
    //access the disk location in FCFS
    case 1: accessFCFS(request, numRequest);
        break;

    //access the disk location in SSTF
    case 2: accessSSTF(request, numRequest);
        break;

        //access the disk location in SCAN
     case 3: accessSCAN(request, numRequest);
        break;

        //access the disk location in CSCAN
    case 4: accessCSCAN(request,numRequest);
        break;

    //access the disk location in LOOK
    case 5: accessLOOK(request,numRequest);
        break;

    //access the disk location in CLOOK
    case 6: accessCLOOK(request,numRequest);
        break;

    default:
        break;
    }
    return 0;
}