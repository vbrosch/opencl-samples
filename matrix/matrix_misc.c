#define RANDOM_SEED 50

void print_matrix(int* mat, int size){
  printf("{");
  for(int i=0; i < size; i++){
    printf("{");
    for(int j=0; j < size; j++){
      printf("%d,", mat[i * size + j]);
    }
    printf("}");
  }
  printf("}\n");
}

void print_vector(int* vec, int size){
  printf("{");
  for(int j=0; j < size; j++){
    printf("%d,", vec[j]);
  }
  printf("}\n");
}

int* allocate_matrix(int size){
  return (int*) malloc(sizeof(int) * size * size);
}

int* allocate_vector(int size){
  return (int*) malloc(sizeof(int) * size);
}

int* get_random_matrix(int size){
  int* matrix = allocate_matrix(size);
  srand(time(NULL));

  for(int row_i=0; row_i < size; row_i++){
    for(int col_i=0; col_i < size; col_i++){
      matrix[(row_i * size) + col_i] = rand() % RANDOM_SEED + 1;
    }
  }

  return matrix;
}

int* get_random_vector(int size){
  int* vector = allocate_vector(size);

  for(int i = 0; i < size; i++){
    vector[i] = rand() % RANDOM_SEED;
  }

  return vector;
}

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

int* matrix_vector_mult(int* a, int* v, int size){
  int* result = allocate_vector(size);

  for (int r_i = 0; r_i < size; r_i++) {
    result[r_i] = 0;

    for(int c_i = 0; c_i < size; c_i++){
      result[r_i] += a[r_i * size + c_i] * v[c_i];
    }
  }

  return result;
}
