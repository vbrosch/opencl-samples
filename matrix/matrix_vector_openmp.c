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

  int* c = allocate_vector(dim_matrix);

  #pragma omp parallel for schedule(static) shared(a, dim_matrix, v, c)
  for (int r_i = 0; r_i < dim_matrix; r_i++) {
    for(int c_i = 0; c_i < dim_matrix; c_i++){
      c[r_i] += a[r_i * dim_matrix + c_i] * v[c_i];
    }
  }

  end = clock();
  time_used = ((double) end - start) / CLOCKS_PER_SEC * 1000.0f;

  printf("Multiplication took %f ms.", time_used);

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
