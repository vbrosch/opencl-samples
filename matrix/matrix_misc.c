#define RANDOM_SEED 20

int* allocate_matrix(int size){
  return (int*) malloc(sizeof(int) * size * size);
}

int* get_random_matrix(int size){
  int* matrix = allocate_matrix(size);
  srand(time(NULL));

  for(int row_i=0; row_i < size; row_i++){
    for(int col_i=0; col_i < size; col_i++){
      matrix[(row_i * size) + col_i] = rand() % RANDOM_SEED;
    }
  }

  return matrix;
}
