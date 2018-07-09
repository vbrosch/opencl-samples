#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix_misc.c"
#include <pthread.h>

struct range_t {
  int thread_id;
  int start;
  int end;
  int size;
  int *matrix;
  int *vector;
  int *result;
};

typedef struct range_t range_t;

void* thread_func(void* args) {
  range_t* range = (range_t*) args;

  for (int r_i = range->start; r_i < range->end; r_i++) {
    int result = 0;
    for(int c_i = 0; c_i < range->size; c_i++){
      result += range->matrix[r_i * range->size + c_i] * range->vector[c_i];
    }
    range->result[r_i] = result;
  }

  return NULL;
}

int main(int argc, char** argv){
  int dim_matrix = 16384 * 2;
  int number_of_threads = 4;
  pthread_t threads[number_of_threads];
  range_t ranges[number_of_threads];

  printf("calculate the product of A . V = R. Matrix dimension %d x %d with %d threads, Vector Dimension %d\n",
    dim_matrix, dim_matrix, number_of_threads, dim_matrix);

  int* a = get_random_matrix(dim_matrix);
  printf("initialized matrix A\n");

  int* v = get_random_vector(dim_matrix);
  printf("initialized vector V\n");

  int* c = allocate_vector(dim_matrix);

  clock_t start, end;
  double time_used;

  start = clock();

  for(int i=0; i < number_of_threads; i++){
    int range_size = dim_matrix / number_of_threads;
    int range_start = i * range_size;
    int range_end = range_start + range_size;

    range_t* r = &ranges[i];
    r->thread_id = i;
    r->matrix = a;
    r->vector = v;
    r->start = range_start;
    r->end = range_end;
    r->size = dim_matrix;
    r->result = c;

    pthread_create(&threads[i], NULL, thread_func, r);
  }

  for(int i=0; i < number_of_threads; i++){
    pthread_join(threads[i], NULL);
  }

  end = clock();
  time_used = ((double) end - start) / CLOCKS_PER_SEC * 1000.0f;

  printf("Multiplication took %f ms.\n", time_used);

  int *c_expected = matrix_vector_mult(a, v, dim_matrix);

  for(int i=0; i < dim_matrix; i++){
    if(c_expected[i] != c[i]){
      printf("%d: Expected %d got %d \n", i, c_expected[i], c[i]);
    }
  }

  free(a);
  free(v);
  free(c);
  free(c_expected);
}
