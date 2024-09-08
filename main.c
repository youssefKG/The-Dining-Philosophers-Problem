#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define PHILO_NUM 5 // Number of philosophers
#define HUNGRY 0    // State when the philosopher is hungry
#define EATING 2    // State when the philosopher is eating
#define THINKING 1  // State when the philosopher is thinking
#define LEFT (philosopher_number + 1) % PHILO_NUM  // Index of the left fork
#define RIGHT (philosopher_number + 5) % PHILO_NUM // Index of the right fork

int philosopheres[5] = {0, 0, 0, 0, 0}; // Initial state of each philosopher

// Semaphore to control the number of philosophers at the table
sem_t philosophers_sem;

// Condition variable for forks, shared by all philosophers
pthread_cond_t forks_cond;

// Mutex locks for each fork
pthread_mutex_t forks_mutex[PHILO_NUM];

// Mutex to protect access to the philosopher states
pthread_mutex_t philo_mutex;

// Function declarations
void pickup_forks(int philosopher_number);
void return_forks(int philosopher_number);
void update_philosophers_state(int philo_num, int state);
void start_eating(int philo_number);
void start_thinking(int philo_number);
void *philosophers_routine(void *args);

int main() {
  // Initialize semaphore to limit number of philosophers at the table
  sem_init(&philosophers_sem, 0, PHILO_NUM - 1);

  // Initialize mutex locks for each fork
  for (int i = 0; i < PHILO_NUM; i++)
    pthread_mutex_init(forks_mutex + i, NULL);

  // Initialize a mutex for philosopher state changes
  pthread_mutex_init(&philo_mutex, NULL);

  // Initialize a condition variable for fork access
  pthread_cond_init(&forks_cond, NULL);

  // Array of philosopher threads
  pthread_t philosophers_threads[PHILO_NUM];

  // Create philosopher threads
  for (int i = 0; i < 5; i++) {
    int *n = malloc(sizeof(int));
    *n = i; // Assign philosopher number
    pthread_create(philosophers_threads + i, NULL, philosophers_routine, n);
  }

  // Wait for all philosopher threads to finish
  for (int i = 0; i < 5; i++)
    pthread_join(philosophers_threads[i], NULL);

  // Cleanup: destroy condition variable and mutexes
  pthread_cond_destroy(&forks_cond);
  for (int i = 0; i < PHILO_NUM; i++)
    pthread_mutex_destroy(forks_mutex + i);

  pthread_mutex_destroy(&philo_mutex);
  sem_destroy(&philosophers_sem);
  return 0;
}

// Function for a philosopher to attempt to pick up forks
void pickup_forks(int philosopher_number) {
  int current_philo = philosopheres[philosopher_number];

  // Wait until both the left and right forks are available
  while ((philosopheres[RIGHT] == EATING || philosopheres[LEFT] == EATING) &&
         current_philo == HUNGRY) {
    printf("Philosopher %d is waiting for forks.\n", philosopher_number);
    pthread_cond_wait(&forks_cond,
                      forks_mutex + philosopher_number); // Wait for forks
  }
}

// Function to update a philosopher's state
void update_philosophers_state(int philo_num, int state) {
  pthread_mutex_lock(&philo_mutex);   // Lock access to philosopher state
  philosopheres[philo_num] = state;   // Update philosopher's state
  pthread_mutex_unlock(&philo_mutex); // Unlock after updating
}

// Function to return forks after eating
void return_forks(int philosopher_number) {
  pthread_mutex_unlock(forks_mutex + RIGHT); // Release the right fork
  pthread_mutex_unlock(forks_mutex + LEFT);  // Release the left fork
}

// Function to simulate a philosopher eating
void start_eating(int philo_number) {
  int sleepingTime = rand() % 4 + 1;               // Random time for eating
  update_philosophers_state(philo_number, EATING); // Update state to eating
  printf("Philosopher %d starts eating.\n", philo_number);
  sleep(sleepingTime); // Simulate eating time
}

// Function to simulate a philosopher thinking
void start_thinking(int philo_number) {
  int thinking_time = rand() % 6 + 1;                // Random time for thinking
  update_philosophers_state(philo_number, THINKING); // Update state to thinking
  printf("Philosopher %d starts thinking.\n", philo_number);
  sleep(thinking_time); // Simulate thinking time
  printf("Philosopher %d finishes thinking.\n", philo_number);
}

// Routine for each philosopher thread
void *philosophers_routine(void *args) {
  int *philo_num = (int *)args; // Extract philosopher number

  while (1) { // Infinite loop for the philosopher's lifecycle
    if (philosopheres[*philo_num] == THINKING) {
      start_thinking(*philo_num); // Philosopher starts thinking
    }

    sem_wait(&philosophers_sem); // Wait for a spot at the table
    update_philosophers_state(*philo_num, HUNGRY); // Philosopher gets hungry
    printf("Philosopher %d is hungry.\n", *philo_num);

    // Try to pick up forks
    pickup_forks(*philo_num);
    printf("Philosopher %d picked up both forks.\n", *philo_num);

    // Start eating after acquiring forks
    start_eating(*philo_num);

    // After eating, the philosopher goes back to thinking
    update_philosophers_state(*philo_num, THINKING);

    // Return forks after eating
    return_forks(*philo_num);

    sem_post(&philosophers_sem); // Release the spot at the table
  }
}
