#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix_misc.c"

int* dot_omp(int* a, int* b, int size){
  int* c = allocate_matrix(size);

  #pragma omp parallel shared(a, b, c, size)
  {
    #pragma omp for schedule(static)
    for(int i=0; i < size; i++){
      for(int j=0; j < size; j++){
        int sum = 0;

        for(int k=0; k < size; k++){
          sum += a[i*size+k] * b[k*size+j];
        }
        c[i * size + j] = sum;
      }
    }
  }

  return c;
}

int main(int argc, char** argv){
  int dim_matrix = 2048;

  printf("calculate the product of A . B = M. Matrix dimensions %d x %d\n",
    dim_matrix, dim_matrix);

  int* a = get_random_matrix(dim_matrix);
  printf("initialized matrix A\n");

  int* b = get_random_matrix(dim_matrix);
  printf("initialized matrix B\n");

  time_t start, end;
  double time_used;

  start = time(NULL);

  int* c = dot_omp(a, b, dim_matrix);

  end = time(NULL);
  time_used = ((double) end - start);

  printf("Addition took %f secs.", time_used);

  free(a);
  free(b);
  free(c);
}
