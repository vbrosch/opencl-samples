#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include "reduce_misc.c"

typedef struct {
  int start;
  int end;
  int (*op) (int, int);
  int* data;
  int result;
} thread_t;

int reduce(int *data, int start, int end, int (*op) (int, int)){
  int result = 0;
  for(int i=start; i < end; i++) {
    result = op(result, data[i]);
  }

  return result;
}

void* reduce_thread(void* args){
  thread_t* range = (thread_t*) args;
  range->result = reduce(range->data, range->start, range->end, range->op);
}

int reduce_parallel(int thread_count, int *data, int count, int (*op) (int, int)){
  int result;
  pthread_t t[thread_count];
  int chunk_size = count / thread_count;
  thread_t r[thread_count];

  for(int i = 0; i < thread_count; i++){
    int start = i*chunk_size;
    thread_t range = {start, start + chunk_size, op, data, 0};
    r[i] = range;

    pthread_create(&t[i], NULL, reduce_thread, &r[i]);
  }

  int results[thread_count];

  for(int i=0; i < thread_count; i++){
    pthread_join(t[i], NULL);
    results[i] = r[i].result;
  }

  return reduce(results, 0, thread_count, op);
}

int main(int argc, char** argv){
  int count = 262144;
  int* data = (int*) malloc(sizeof(int) * count);
  int thread_count = 4;

  generate_data(data, count);

  clock_t start, end;

  start = clock();

  int max_el = reduce_parallel(thread_count, data, count, max);
  int sum_el = reduce_parallel(thread_count, data, count, sum);

  end = clock();

  double duration = ((double)(end - start))/CLOCKS_PER_SEC * 1000.0f;

  printf("max: %d, sum: %d\n", max_el, sum_el);
  printf("time: %fms\n", duration);

  int max_el_seq = reduce_seq(data, count, max);
  int sum_el_seq = reduce_seq(data, count, sum);

  if(max_el != max_el_seq){
    printf("ERR! Sequential result differs from seq result max: %d\n", max_el_seq);
  }

  if(sum_el != sum_el_seq){
    printf("ERR! Sequential result differs from pthreads result sum: %d\n", sum_el_seq);
  }

  free(data);
}
