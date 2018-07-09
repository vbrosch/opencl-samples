__kernel void matrix_vector_mult(__global int *a, __global int *v, __global int *r,
    int size){
        int row = get_global_id(0);

        __private int sum = 0;

        for(int c_i=0; c_i < size; c_i++) {
          sum += a[row*size+c_i] * v[c_i];
        }

        r[row] = sum;
    }
