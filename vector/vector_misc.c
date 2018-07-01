#include <stdio.h>
#include <stdlib.h>

#define RANDOM_SEED 1000000

int* allocate_vector(int size){
  return (int*) malloc(sizeof(int) * size);
}

int* vector_add(int size, int* a, int* b) {
  int* v = allocate_vector(size);

  for(int i=0; i < size; i++){
    v[i] = a[i] + b[i];
  }

  return v;
}

int* get_random_vector(int size) {
  int* v = allocate_vector(size);
  srand(time(NULL));

  for(int i=0; i < size; i++){
    v[i] = rand() % RANDOM_SEED;
  }

  return v;
}
