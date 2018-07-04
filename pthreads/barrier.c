#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define SLEEP_MS  1000000

typedef struct {
  int  t1_result;
  int  t2_result;
  int  result;
  pthread_barrier_t barrier;
} Result;

void* t1_func(void* args){
  Result* arg = (Result*) args;
  usleep(rand() % SLEEP_MS);

  printf("--t1--\n");

  arg->t1_result = 50;

  printf("t1 - before barrier\n");
  pthread_barrier_wait(&arg->barrier);
  printf("t1 - after barrier\n");

  return NULL;
}

void* t2_func(void* args){
  Result* arg = (Result*) args;
  usleep(rand() % SLEEP_MS);

  printf("--t2--\n");

  arg->t2_result = 100;

  printf("t2 - before barrier\n");
  pthread_barrier_wait(&arg->barrier);
  printf("t2 - after barrier\n");

  return NULL;
}

void* t3_func(void* args){
  Result* arg = (Result*) args;
  usleep(rand() % (SLEEP_MS-SLEEP_MS/2));

  printf("--t3--\n");

  printf("t3 - before barrier\n");
  pthread_barrier_wait(&arg->barrier);
  printf("t3 - after barrier\n");

  arg->result = arg->t1_result + arg->t2_result;

  return NULL;
}

int main(int argc, char** argv){
  pthread_t t1, t2, t3;

  srand(time(NULL));

  Result* r = (Result*) malloc(sizeof(Result));

  pthread_barrier_init(&r->barrier, NULL, 3);

  pthread_create(&t1, NULL, t1_func, r);
  pthread_create(&t2, NULL, t2_func, r);
  pthread_create(&t3, NULL, t3_func, r);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);

  printf("Result is %d \n", r->result);

  free(r);
}
