#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include "reduce_misc.c"

int reduce_openmp_sum(int* data, int count, int (*op)(int, int)){
  int result = 0;
  int i = 0;

  #pragma omp parallel for default(shared) private(i) schedule(static) \
          reduction(+:result)
  for(int i=0; i < count; i++){
    result = op(result, data[i]);
  }

  return result;
}

int reduce_openmp_max(int* data, int count, int (*op)(int, int)){
  int result = 0;
  int i = 0;

  #pragma omp parallel for default(shared) private(i) schedule(static)
  for(int i=0; i < count; i++){
    result = op(result, data[i]);
  }

  return result;
}

int main(int argc, char** argv){
  int count = 262144;
  int* data = (int*) malloc(sizeof(int) * count);

  generate_data(data, count);

  clock_t start, end;

  start = clock();

  int max_el = reduce_openmp_max(data, count, max);
  int sum_el = reduce_openmp_sum(data, count, sum);

  end = clock();

  double duration = ((double)(end - start))/CLOCKS_PER_SEC * 1000.0f;

  printf("max: %d, sum: %d\n", max_el, sum_el);
  printf("time: %fms\n", duration);

  int max_el_seq = reduce_seq(data, count, max);
  int sum_el_seq = reduce_seq(data, count, sum);

  if(max_el != max_el_seq){
    printf("ERR! Sequential result differs from seq result max: %d\n", max_el_seq);
  }

  if(sum_el != sum_el_seq){
    printf("ERR! Sequential result differs from pthreads result sum: %d\n", sum_el_seq);
  }

  free(data);
}
