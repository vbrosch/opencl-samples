int sum(int a, int b){
  return a+b;
}

int max(int a, int b){
  return (a>b) ? a : b;
}

void generate_data(int* data, int count){
  srand(time(NULL));

  for(int i=0; i < count; i++){
    data[i] = random() % 1000;
  }
}

int reduce_seq(int *data, int count, int (*op) (int, int)){
  int result = data[0];
  for(int i=1; i < count; i++) {
    result = op(result, data[i]);
  }

  return result;
}
