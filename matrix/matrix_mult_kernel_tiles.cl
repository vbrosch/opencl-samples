#define TILE_WIDTH 8

__kernel void matrix_mult_tiles(__global int *a, __global int *b, __global int *c,
    int size){
        __local int a_local[TILE_WIDTH][TILE_WIDTH];
        __local int b_local[TILE_WIDTH][TILE_WIDTH];

        __private int sum = 0.0;

        int local_0 = get_local_id(0);
        int local_1 = get_local_id(1);

        int global_0 = get_global_id(0);
        int global_1 = get_global_id(1);

        int group_0 = get_group_id(0);
        int group_1 = get_group_id(1);

        for(int m=0; m < size/TILE_WIDTH; m++){

          a_local[local_0][local_1] = a[global_0*size + (m*TILE_WIDTH) + local_1];
          b_local[local_0][local_1] = b[(m*TILE_WIDTH+local_0)*size+global_1];

          barrier(CLK_LOCAL_MEM_FENCE);

          for(int k=0; k < TILE_WIDTH; k++){
            sum += a_local[local_0][k] * b_local[k][local_1];
          }

          barrier(CLK_LOCAL_MEM_FENCE);
        }

        c[global_0 * size + global_1] = sum;
    }
