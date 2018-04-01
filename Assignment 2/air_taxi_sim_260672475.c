/*
 ----------------- COMP 310/ECSE 427 Winter 2018 -----------------
 Dimitri Gallos
 Assignment 2 skeleton
 
 -----------------------------------------------------------------
 I declare that the awesomeness below is a genuine piece of work
 and falls under the McGill code of conduct, to the best of my knowledge.
 -----------------------------------------------------------------
 */

//Please enter your name and McGill ID below
//Name: Alex Hale
//McGill ID: 260672475

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <semaphore.h>

int BUFFER_SIZE = 100; //size of queue
sem_t taxi_line_available;  // number of empty spots in the buffer
                          // when 0, buffer is full
sem_t taxi_line_taken;   // number of full spots in the buffer
                          // when 0, buffer is empty
pthread_mutex_t buffer_mutex;   // mutex variable to ensure safe access to queue

volatile int quit = 0;  // used to quit the program if needed

// A structure to represent a queue
struct Queue {
    int front, rear, size;
    unsigned capacity;
    int* array;
};
 
// function to create a queue of given capacity. 
// It initializes size of queue as 0
struct Queue* createQueue(unsigned capacity) {
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;  // This is important, see the enqueue
    queue->array = (int*) malloc(queue->capacity * sizeof(int));
    return queue;
}
 
// Queue is full when size becomes equal to the capacity 
int isFull(struct Queue* queue) {
    return ((queue->size ) >= queue->capacity);
}
 
// Queue is empty when size is 0
int isEmpty(struct Queue* queue) {
    return (queue->size == 0);
}
 
// Function to add an item to the queue.  
// It changes rear and size
void enqueue(struct Queue* queue, int item) {
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1)%queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
    // printf("%d enqueued to queue\n", item);
}

// Function to remove an item from queue. 
// It changes front and size
int dequeue(struct Queue* queue) {
    if (isEmpty(queue))
        return INT_MIN;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1)%queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

// Function to get front of queue
int front(struct Queue* queue) {
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->front];
}
 
// Function to get rear of queue
int rear(struct Queue* queue) {
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->rear];
}

// Function to print each element in the queue
void print(struct Queue* queue) {
    if (queue->size == 0){
        return;
    }
    
    for (int i = queue->front; i < queue->front +queue->size; i++){
        
        printf("Element at position %d is %d \n ", i % (queue->capacity ), queue->array[i % (queue->capacity)]);
    }
    
}

struct Queue* queue;

/*Producer Function: Simulates an Airplane arriving and dumping 5-10 passengers to the taxi platform */
void *FnAirplane(void* cl_id) {
  // print about this airplane thread being created
  int plane_number = *(int *)cl_id;
  printf("Creating airplane thread %d.\n", plane_number);

  // loop until the program gets quit
  while (!quit) {
    int n = (rand() % 5) + 5;   // number of passengers from this plane that want taxis (5 to 10)

    printf("Airplane %d arrives with %d passengers.\n", plane_number, n);

    for (int i = 0; i < n; i++) {
      // passenger integer:
        // first digit: 1
        // next three digits: plane number
        // last three digits: passenger number
      int passenger = 1000000 + (plane_number * 1000) + i;

      int lineStatus;
      sem_getvalue(&taxi_line_available, &lineStatus);
      if (!lineStatus) {
        // line for taxis is full, passenger doesn't join => discard
        // print message about rest of this plane's passengers taking bus
        printf("Platform is full. Rest of passengers of plane %d take the bus.\n", plane_number);
        i = n;  // exit for loop
      } else {
        // decrement the number of spots available in line
          // since we already confirmed there weren't 0 spots available (using conditional statement above), we know that the thread won't wait
        sem_wait(&taxi_line_available);

        pthread_mutex_lock(&buffer_mutex);   // protect queue
        enqueue(queue, passenger);
        printf("Passenger %d of airplane %d enters the queue.\n", passenger, plane_number); // print about passenger entering queue
        pthread_mutex_unlock(&buffer_mutex);  // release queue

        // indicate that one more spot in the line is taken
          // will wake up consumer if they were waiting for passengers to become available
        sem_post(&taxi_line_taken);
      }
    }
    // printf("Airplane %d sleeping for 1 second.\n", plane_number); // testing only
    usleep(1*1000000);   // sleep for 1 hour = 1 second
  }
  return NULL;
}

/* Consumer Function: simulates a taxi that takes n time to take a passenger home and come back to the airport */
void *FnTaxi(void* pr_id) {
  int taxi_number = *(int *)pr_id;

  while (!quit) {
    // print about this taxi arriving to the line of available taxis
    printf("Taxi driver %d arrives.\n", taxi_number);

    int lineStatus;
    sem_getvalue(&taxi_line_taken, &lineStatus);
    if (!lineStatus) {
      // line for taxis is empty, print appropriate message
      printf("Taxi driver %d waits for passengers to enter the platform.\n", taxi_number);
    }

    // if the buffer is empty, let the producer run until it becomes non-empty
    sem_wait(&taxi_line_taken);

    pthread_mutex_lock(&buffer_mutex);
    int passenger = dequeue(queue);
    pthread_mutex_unlock(&buffer_mutex);

    // increase the number of spots available in the taxi line
    sem_post(&taxi_line_available);

    // waiting time (in minutes) before this taxi is available again
    int sleepLength = (rand() % 20) + 10;  // generate a random number between 10 and 30 minutes

    // print about which taxi is taking which passenger
    printf("Taxi driver %d picked up client %d from the platform. Trip length %d minutes.\n", taxi_number, passenger, sleepLength);

    // sleep this thread for the appropriate amount of time (converting to a fraction of a second between 1/6 to 1/2 seconds)
    float f = (float)sleepLength / 60;
    // printf("Taxi %d sleeping for %f seconds.\n", taxi_number, f);   // testing only
    usleep(f*1000000);
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  int num_airplanes;
  int num_taxis;

  num_airplanes=atoi(argv[1]);
  num_taxis=atoi(argv[2]);
  
  printf("You entered: %d airplanes per hour\n", num_airplanes);
  printf("You entered: %d taxis\n", num_taxis);
   
  //initialize queue
  queue = createQueue(BUFFER_SIZE);
  
  //declare arrays of threads and initialize semaphore(s)
  pthread_mutex_init(&buffer_mutex, NULL);
  sem_init(&taxi_line_available, 0, BUFFER_SIZE);
  sem_init(&taxi_line_taken, 0, 0);
  pthread_t planeThreads[num_airplanes];
  pthread_t taxiThreads[num_taxis];

  //create arrays of integer pointers to ids for taxi / airplane threads
  int *taxi_ids[num_taxis];
  int *airplane_ids[num_airplanes];

  //create threads for airplanes
  for (int i = 0; i < num_airplanes; i++) {
    airplane_ids[i] = malloc(sizeof(int));
    *airplane_ids[i] = i;
    if (pthread_create(&planeThreads[i], NULL, FnAirplane, airplane_ids[i]) != 0) {
      printf("Error with Plane %d's thread creation.", i);
    }
  }

  //create threads for taxis
  for (int i = 0; i < num_taxis; i++) {
    taxi_ids[i] = malloc(sizeof(int));
    *taxi_ids[i] = i; 
    if (pthread_create(&taxiThreads[i], NULL, FnTaxi, taxi_ids[i]) != 0) {
      printf("Error with Taxi %d's thread creation.", i);
    }
  }
  
  pthread_mutex_destroy(&buffer_mutex);
  sem_destroy(&taxi_line_available);
  sem_destroy(&taxi_line_taken);
  pthread_exit(NULL);
}
