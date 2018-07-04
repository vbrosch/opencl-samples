__kernel void reduce_sum_kernel(__global int *data, __global int *result,
    int length)
    {
        __local float sum[32];
        int id = get_local_id(0);

        sum[id] = data[get_global_id(0)];

        for(int stride = get_local_size(0) / 2; stride > 0; stride /= 2){
          barrier(CLK_LOCAL_MEM_FENCE);

          if(id < stride){
            sum[id] += sum[id + stride];
          }
        }

        barrier(CLK_LOCAL_MEM_FENCE);

        result[get_group_id(0)] = sum[0];
    }
