#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix_misc.c"

int* dot(int* a, int* b, int size){
  int* c = allocate_matrix(size);

  for(int i=0; i < size; i++){
    for(int j=0; j < size; j++){
      int sum = 0;

      for(int k=0; k < size; k++){
        sum += a[i*size+k] * b[k*size+j];
      }
      c[i * size + j] = sum;
    }
  }

  return c;
}

int main(int argc, char** argv){
  int dim_matrix = 1024;

  printf("calculate the product of A . B = M. Matrix dimensions %d x %d\n",
    dim_matrix, dim_matrix);

  int* a = get_random_matrix(dim_matrix);
  printf("initialized matrix A\n");

  int* b = get_random_matrix(dim_matrix);
  printf("initialized matrix B\n");

  time_t start, end;
  double time_used;

  start = time(NULL);

  int *c = dot(a, b, dim_matrix);

  end = time(NULL);
  time_used = (double) end - start;

  printf("Addition took %f seconds.", time_used);

  free(a);
  free(b);
  free(c);
}
