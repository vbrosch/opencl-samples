#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 2
#define SLEEP_MS  1000000

typedef struct {
  char buffer[BUFFER_SIZE];
  int  buffer_pos;
  int  capacity;
  pthread_mutex_t mutex;
  pthread_cond_t sig;
} stack_t;

typedef struct {
  stack_t* stack;
  int id;
} thread_t;

void push(int thread_id, stack_t* stack, char a){
  pthread_mutex_lock(&stack->mutex);

  while(stack->buffer_pos >= stack->capacity){
    printf("#P %d: ERR! Stack is full.\n", thread_id);
    pthread_cond_wait(&stack->sig, &stack->mutex);
  }

  stack->buffer[stack->buffer_pos] = a;
  stack->buffer_pos = stack->buffer_pos + 1;
  pthread_cond_signal(&stack->sig);

  pthread_mutex_unlock(&stack->mutex);
}

void pop(int thread_id, stack_t* stack){
  pthread_mutex_lock(&stack->mutex);

  while(stack->buffer_pos <= 0){
    printf("#C %d: ERR! Stack is empty.\n", thread_id);
    pthread_cond_wait(&stack->sig, &stack->mutex);
  }

  char c = stack->buffer[stack->buffer_pos-1];
  stack->buffer_pos = stack->buffer_pos - 1;

  printf("## C %d: read %c from buffer \n", thread_id, c);
  pthread_cond_signal(&stack->sig);

  pthread_mutex_unlock(&stack->mutex);
}

void* producer_func(void* args){
  printf("## P: called producer_func()\n");

  thread_t* thread = (thread_t*) args;

  while(1){
    char random_char = 'A' + (random() % 26);
    printf("## P %d: writing %c to buffer at pos %d \n", thread->id,
      random_char, thread->stack->buffer_pos);
    push(thread->id, thread->stack, random_char);
    usleep(random() % SLEEP_MS);
  }

  return NULL;
}

void* consumer_func(void* args){
  thread_t* thread = (thread_t*) args;
  usleep(SLEEP_MS);

  while(1){
    pop(thread->id, thread->stack);
    usleep(random() % SLEEP_MS);
  }
}

int main(int argc, char** argv){
  pthread_t producer_1;
  pthread_t consumer_1;

  srand(time(NULL));

  stack_t* stack = (stack_t*) malloc(sizeof(stack_t));
  stack->buffer_pos = 0;
  stack->capacity = BUFFER_SIZE;

  thread_t first_producer_thread_args = {stack, 0};
  thread_t first_consumer_thread_args = {stack, 0};

  pthread_mutex_init(&stack->mutex, NULL);
  pthread_cond_init(&stack->sig, NULL);

  pthread_create(&producer_1, NULL, producer_func, &first_producer_thread_args);
  pthread_create(&consumer_1, NULL, consumer_func, &first_consumer_thread_args);

  pthread_join(consumer_1, NULL);
  pthread_join(producer_1, NULL);

  free(stack);
}
