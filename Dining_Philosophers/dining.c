#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_PHILOSOPHERS 5

struct philstruct{
  int eating;
  int thinking;
  int hungry;
} phil[NUM_PHILOSOPHERS];

pthread_mutex_t mutex;
pthread_cond_t cond_var;
pthread_attr_t attr;

void *philosophers(int i);
void pickup_forks(int i);
void return_forks(int i);
void checkforks(int i);

int main (void) {
  
  pthread_t threads[NUM_PHILOSOPHERS];
  time_t t;

  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond_var, NULL);
  pthread_attr_init(&attr);

  srand((unsigned)time(&t));

  int i, threadinit;
  printf("Lets think...\n");

  for (i=0; i<5; i++) {
    threadinit = pthread_create(&threads[i], NULL, (void*)philosophers, (void*)i);
    if(threadinit == 1) {
      printf("Error initializing threads\n");
      exit(-1);
    }
  }

  for (i=0;i<NUM_PHILOSOPHERS; i++){
    pthread_join(threads[i], NULL);
  }
  return 1;
}	
      

void *philosophers(int i){
  printf("Philosopher %d is thinking\n", i+1);
  usleep(rand()%4*100000);
  printf("Philosopher %d is hungry\n", i+1);
  pickup_forks(i);
  pthread_exit(NULL);
  return;
}

void pickup_forks(int i) {
  checkfork(i);
  printf("Philosopher %d picked up forks\n", i+1);
  // Philosoper eats for 1-3 seconds
  usleep((rand() % 4) * 100000);
  return_forks(i);
  return;
}

void return_forks(int i) {
  phil[i].eating = 0;
  printf("Philosopher %d put down forks\n", i+1);
  pthread_mutex_unlock(&mutex);
  return;
}

void checkfork(int i){
    if(phil[(i+4)%5].eating == 1 && phil[(i+1)%5].eating == 1){
      pthread_cond_wait(&mutex, &cond_var);
    }

    pthread_mutex_lock(&mutex);
    phil[i].eating = 1;
    pthread_cond_signal(&cond_var);

  return;
}
