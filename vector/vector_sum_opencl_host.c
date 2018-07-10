#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <CL/opencl.h>
#include "../misc/opencl_utils.c"
#include "vector_misc.c"
#include <math.h>

int main(int argc, char** argv){
  int dim_vector = pow(2,26);
  int number_of_workitems = dim_vector;
  int number_of_workitems_in_workgroup = 1;
  int workgroup_count = number_of_workitems / number_of_workitems_in_workgroup;

  printf("Will launch %d work items in %d work groups (wg size = %d)\n",
  number_of_workitems, workgroup_count, number_of_workitems_in_workgroup);

  int err;

  int* v1 = get_random_vector(dim_vector);
  printf("initialized vector v1\n");

  int* v2 = get_random_vector(dim_vector);
  printf("initialized vector v2\n");

  clock_t start, end;
  double time_used;

  start = clock();

  // get kernel source
  char* kernel_source = read_kernel_file("vector_sum_kernel.cl");

  cl_device_id device_id;             // compute device id
  cl_context context;                 // compute context
  cl_command_queue commands;          // compute command queue
  cl_program program;                 // compute program
  cl_kernel kernel;                   // compute kernel
  char* device_name[128];

  cl_mem v1_mem;
  cl_mem v2_mem;
  cl_mem c_mem;

  size_t global_size[] = {number_of_workitems};
  size_t local_size[] = {number_of_workitems_in_workgroup};

  size_t size_vector = sizeof(int) * dim_vector;
  int* c = (int*) malloc(size_vector);

  int gpu = 1;

  err = clGetDeviceIDs(NULL, gpu ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU,
    1, &device_id, NULL);

  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to create a device group!\n");
    return EXIT_FAILURE;
  }

  err = clGetDeviceInfo(device_id, CL_DEVICE_NAME, sizeof(device_name), &device_name, NULL);

  if(err != CL_SUCCESS){
    printf("Error: Could not get device info!\n");
    return EXIT_FAILURE;
  }

  printf("Selected device: %s\n", device_name);

  context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);

  if(!context){
    printf("Error: Failed to create a compute context!\n");
    return EXIT_FAILURE;
  }

  commands = clCreateCommandQueue(context, device_id, 0, &err);
  if (!commands)
  {
    printf("Error: Failed to create a command commands!\n");
    return EXIT_FAILURE;
  }

  // Create the compute program from the source buffer
  //
  program = clCreateProgramWithSource(context, 1, (const char **) & kernel_source, NULL, &err);
  if (!program)
  {
    printf("Error: Failed to create compute program!\n");
    return EXIT_FAILURE;
  }

  // Build the program executable
  //
  err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  if (err != CL_SUCCESS)
  {
      size_t len;
      char buffer[2048];

      printf("Error: Failed to build program executable!\n");
      clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
      printf("%s\n", buffer);
      exit(1);
  }

  printf("Successfully created context, command queue and compiled program\n");

  // Create the compute kernel in the program we wish to run
  //
  kernel = clCreateKernel(program, "vector_sum", &err);
  if (!kernel || err != CL_SUCCESS)
  {
    printf("Error: Failed to create compute kernel!\n");
    exit(1);
  }

  printf("Successfully created kernel.\n");

  // Create the input and output arrays in device memory for our calculation
  //
  v1_mem = clCreateBuffer(context,  CL_MEM_COPY_HOST_PTR,  size_vector, v1, NULL);
  v2_mem = clCreateBuffer(context,  CL_MEM_COPY_HOST_PTR,  size_vector, v2, NULL);
  c_mem = clCreateBuffer(context,  CL_MEM_WRITE_ONLY, size_vector, NULL, NULL);

  if (!v1_mem || !v2_mem || !c_mem)
  {
      printf("Error: Failed to allocate device memory!\n");
      exit(1);
  }

  printf("Successfully allocated device memory for V1, V2 and result vector C.\n");

  err = 0;
  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &v1_mem);
  err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &v2_mem);
  err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &c_mem);

  if (err != CL_SUCCESS)
  {
      printf("Error: Failed to set kernel arguments! %d\n", err);
      exit(1);
  }

  // Execute the kernel over the entire range of our 1d input data set
  // using the maximum number of work group items for this device
  //

  printf("Begin execution of kernel.\n");

  err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, global_size, local_size, 0, NULL, NULL);
  if (err)
  {
      printf("Error: Failed to execute kernel %d!\n", err);
      return EXIT_FAILURE;
  }

  // Wait for the command commands to get serviced before reading back results
  //
  clFinish(commands);

  printf("Kernel executed successfully, will write back results\n");

  err = clEnqueueReadBuffer(commands, c_mem, CL_TRUE, 0, size_vector, c, 0, NULL, NULL);

  if (err != CL_SUCCESS)
  {
      printf("Error: Failed to read output array! %d\n", err);
      exit(1);
  }

  end = clock();
  time_used = ((double) end - start) / CLOCKS_PER_SEC * 1000.0f;

  printf("Addition took %f ms.", time_used);
  printf("Will compare the results now\n");

  int* c_expected = vector_add(dim_vector, v1, v2);

  int c_not_equal_0 = 0;

  for(int i=0; i < dim_vector; i++){
    if(c[i] != 0){
      c_not_equal_0++;
    }
    if(c_expected[i] != c[i]){
      printf("%d: Expected %d got %d \n", i, c_expected[i], c[i]);
    }
  }

  printf("\nComparison finished, c != 0: %d \n", c_not_equal_0);

  clReleaseMemObject(v1_mem);
  clReleaseMemObject(v2_mem);
  clReleaseMemObject(c_mem);

  free(v1);
  free(v2);
  free(c);
  free(c_expected);
}
