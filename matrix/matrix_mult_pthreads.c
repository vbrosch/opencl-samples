#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "matrix_misc.c"

typedef struct range_t range_t;

struct range_t {
  int start;
  int end;
  int* a;
  int* b;
  int* c;
  int size;
};

int* dot_slice(range_t slice){
  for(int i=slice.start; i < slice.end; i++){
    for(int j=0; j < slice.size; j++){
      int sum = 0;

      for(int k=0; k < slice.size; k++){
        sum += slice.a[i*slice.size+k] * slice.b[k*slice.size+j];
      }

      slice.c[i * slice.size + j] = sum;
    }
  }
}

void* dot_thread_func(void* args){
  range_t range = *(range_t*) args;
  dot_slice(range);
}

int main(int argc, char** argv){
  int dim_matrix = 1024;
  int number_of_threads = 4;
  int width_per_thread = dim_matrix / number_of_threads;

  printf("calculate the product of A . B = M. Matrix dimensions %d x %d with %d pthreads.\n",
    dim_matrix, dim_matrix, number_of_threads);

  int* a = get_random_matrix(dim_matrix);
  printf("initialized matrix A\n");

  int* b = get_random_matrix(dim_matrix);
  printf("initialized matrix B\n");

  time_t start, end;
  double time_used;

  start = time(NULL);

  int* c = allocate_matrix(dim_matrix);

  pthread_t t[number_of_threads];
  range_t r[number_of_threads];

  // start pthreads
  for (int i = 0; i < number_of_threads; i++){
    r[i].start = i * width_per_thread;
    r[i].end = r[i].start + width_per_thread - 1;
    r[i].size = dim_matrix;
    r[i].a = a;
    r[i].b = b;
    r[i].c = c;

    pthread_create(&t[i], NULL, dot_thread_func, &r[i]);
  }

  // wait for finished threads
  for (int i=0; i < number_of_threads; i++){
    pthread_join(t[i], NULL);
  }

  end = time(NULL);
  time_used = (double) end - start;

  printf("Addition took %f seconds.", time_used);

  free(a);
  free(b);
  free(c);

}
