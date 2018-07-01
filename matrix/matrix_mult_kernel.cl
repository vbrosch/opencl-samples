__kernel void matrix_mult(__global int *a, __global int *b, __global int *c,
    int size){
        int row = get_global_id(0);
        int col = get_global_id(1);

        __private int sum = 0;

        for(int k=0; k < size; k++){
          sum += a[row*size+k] * b[k*size+col];
        }

        c[row * size + col] = sum;
    }
