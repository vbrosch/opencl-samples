__kernel void vector_sum(__global int *a, __global int *b, __global int* c){
  int row = get_global_id(0);
  c[row] = a[row] + b[row];
}
