#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "reduce_misc.c"

int main(int argc, char** argv){
  int count = 262144;
  int* data = (int*) malloc(sizeof(int) * count);

  generate_data(data, count);

  clock_t start, end;

  start = clock();

  int max_el = reduce_seq(data, count, max);
  int sum_el = reduce_seq(data, count, sum);

  end = clock();

  double duration = ((double)(end - start))/CLOCKS_PER_SEC * 1000.0f;

  printf("max: %d, sum: %d\n", max_el, sum_el);
  printf("time: %fms\n", duration);

  free(data);
}
