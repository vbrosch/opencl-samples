#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define SLEEP_MS  1000000

typedef struct {
  int waiting_threads;
  int num_threads;
  pthread_mutex_t m;
  pthread_cond_t cond;
} barrier_t;

typedef struct {
  int  t1_result;
  int  t2_result;
  int  result;
  barrier_t* barrier;
} result_t;

void init_barrier(barrier_t* barrier, int num_threads){
  barrier->waiting_threads = 0;
  barrier->num_threads = num_threads;

  pthread_mutex_init(&(barrier->m), NULL);
  pthread_cond_init(&(barrier->cond), NULL);
}

void barrier_wait(barrier_t* barrier){
  pthread_mutex_lock(&(barrier->m));
  barrier->waiting_threads++;

  while(barrier->waiting_threads < barrier->num_threads){
    pthread_cond_wait(&(barrier->cond), &(barrier->m));
  }

  pthread_cond_signal(&(barrier->cond));
  pthread_mutex_unlock(&(barrier->m));
}

void* t1_func(void* args){
  result_t* arg = (result_t*) args;
  usleep(rand() % SLEEP_MS);

  printf("--t1--\n");

  arg->t1_result = 50;

  printf("t1 - before barrier\n");
  barrier_wait(arg->barrier);
  printf("t1 - after barrier\n");

  return NULL;
}

void* t2_func(void* args){
  result_t* arg = (result_t*) args;
  usleep(rand() % SLEEP_MS);

  printf("--t2--\n");

  arg->t2_result = 100;

  printf("t2 - before barrier\n");
  barrier_wait(arg->barrier);
  printf("t2 - after barrier\n");

  return NULL;
}

void* t3_func(void* args){
  result_t* arg = (result_t*) args;
  usleep(rand() % (SLEEP_MS-SLEEP_MS/2));

  printf("--t3--\n");

  printf("t3 - before barrier\n");
  barrier_wait(arg->barrier);
  printf("t3 - after barrier\n");

  arg->result = arg->t1_result + arg->t2_result;

  return NULL;
}

int main(int argc, char** argv){
  pthread_t t1, t2, t3;

  srand(time(NULL));

  barrier_t barrier;
  result_t r;
  r.barrier = &barrier;

  init_barrier(&barrier, 3);

  pthread_create(&t1, NULL, t1_func, &r);
  pthread_create(&t2, NULL, t2_func, &r);
  pthread_create(&t3, NULL, t3_func, &r);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);

  printf("result_t is %d \n", r.result);
}
