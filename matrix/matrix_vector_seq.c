#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix_misc.c"

int main(int argc, char** argv){
  int dim_matrix = 16384 * 2;

  printf("calculate the product of A . V = R. Matrix dimension %d x %d, Vector Dimension %d\n",
    dim_matrix, dim_matrix, dim_matrix);

  int* a = get_random_matrix(dim_matrix);
  printf("initialized matrix A\n");

  int* v = get_random_vector(dim_matrix);
  printf("initialized vector V\n");

  clock_t start, end;
  double time_used;

  start = clock();

  int *c = matrix_vector_mult(a, v, dim_matrix);

  end = clock();
  time_used = ((double) end - start) / CLOCKS_PER_SEC * 1000.0f;

  printf("Multiplication took %f ms.", time_used);

  free(a);
  free(v);
  free(c);
}
