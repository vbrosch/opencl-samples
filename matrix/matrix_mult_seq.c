#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix_misc.c"

int main(int argc, char** argv){
  int dim_matrix = 2048;

  printf("calculate the product of A . B = M. Matrix dimensions %d x %d\n",
    dim_matrix, dim_matrix);

  int* a = get_random_matrix(dim_matrix);
  printf("initialized matrix A\n");

  int* b = get_random_matrix(dim_matrix);
  printf("initialized matrix B\n");

  clock_t start, end;
  double time_used;

  start = clock();

  int *c = dot(a, b, dim_matrix);

  end = clock();
  time_used = (double) end - start;

  printf("Addition took %f clocks.", time_used);

  free(a);
  free(b);
  free(c);
}
