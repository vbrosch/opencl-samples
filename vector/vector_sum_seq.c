#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "vector_misc.c"

int main(int argc, char** argv){
  printf("calculate the sum of two vectors\n");

  int dim_vector = 8192 * 2;

  int* a = get_random_vector(dim_vector);
  int* b = get_random_vector(dim_vector);

  clock_t start, end;
  double cpu_time_used;

  start = clock();

  int *c = vector_add(dim_vector, a, b);

  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

  printf("Addition took %f seconds.", cpu_time_used);

  for(int i=0; i < dim_vector; i += 256){
    printf("%d+%d=C[%d]=%d\n", a[i], b[i], i, c[i]);
  }

  free(c);
  free(a);
  free(b);
}
