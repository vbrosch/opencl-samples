#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "vector_misc.c"
#include <pthread.h>

typedef struct range_t range_t;

struct range_t {
  int start;
  int end;
  int* a;
  int* b;
  int* c;
};

int* vector_add_slice(range_t r){
  for(int i=r.start; i < r.end; i++){
    r.c[i] = r.a[i] + r.b[i];
  }
}

void* vector_add_thread_func(void* args){
  range_t r = *(range_t*) args;
  vector_add_slice(r);
}

int main(int argc, char** argv){
  int thread_number = 8;
  int dim_vector = 8192 * 2;

  printf("calculate the sum of two vectors with dimension %d using %d pthreads\n",
    dim_vector, thread_number);

  int* a = get_random_vector(dim_vector);
  int* b = get_random_vector(dim_vector);

  clock_t start, end;
  double cpu_time_used;

  int* r = (int*) malloc(sizeof(int) * dim_vector);

  start = clock();

  pthread_t* threads = (pthread_t*) malloc(sizeof(pthread_t) * thread_number);
  range_t* ranges = (range_t*) malloc(sizeof(range_t) * thread_number);

  for (int i=0; i < thread_number; i++){
    int start = (dim_vector / thread_number) * i;
    int end = ((dim_vector / thread_number) * (i+1)) - 1;

    ranges[i].start = start;
    ranges[i].end = end;
    ranges[i].a = a;
    ranges[i].b = b;
    ranges[i].c = r;

    pthread_create(&threads[i], NULL, vector_add_thread_func, &ranges[i]);
  }

  for(int i=0; i < thread_number; i++){
    pthread_join(threads[i], NULL);
  }

  end = clock();

  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

  printf("Addition took %f seconds.", cpu_time_used);

  for(int i=0; i < dim_vector; i += 1024){
    printf("%d+%d=C[%d]=%d\n", a[i], b[i], i, r[i]);
  }

  free(r);
  free(a);
  free(b);
  free(threads);
  free(ranges);
}
