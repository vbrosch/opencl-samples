#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <CL/opencl.h>
#include "../misc/opencl_utils.c"
#include "matrix_misc.c"

int main(int argc, char** argv){
  int dim_matrix = 2048;
  int number_of_workitems = dim_matrix;
  int number_of_workitems_in_workgroup = 8;
  int workgroup_count = 2 * (number_of_workitems / number_of_workitems_in_workgroup);

  printf("Will launch %d work items in %d work groups (wg size = %d)\n", number_of_workitems*number_of_workitems, workgroup_count, number_of_workitems_in_workgroup);

  int err;

  printf("calculate the product of A . B = M. Matrix dimensions %d x %d with %dx%d workitems.\n",
    dim_matrix, dim_matrix, number_of_workitems, number_of_workitems);

  // calc matrix

  int* a = get_random_matrix(dim_matrix);
  printf("initialized matrix A\n");

  int* b = get_random_matrix(dim_matrix);
  printf("initialized matrix B\n");

  clock_t start, end;
  double time_used;

  start = clock();

  // get kernel source
  char* kernel_source = read_kernel_file("matrix_mult_kernel_tiles.cl");

  cl_device_id device_id;             // compute device id
  cl_context context;                 // compute context
  cl_command_queue commands;          // compute command queue
  cl_program program;                 // compute program
  cl_kernel kernel;                   // compute kernel
  char* device_name[128];

  cl_mem a_mem;
  cl_mem b_mem;
  cl_mem c_mem;

  size_t global_size[] = {number_of_workitems, number_of_workitems};
  size_t local_size[] = {number_of_workitems_in_workgroup, number_of_workitems_in_workgroup};

  size_t size_matrix = sizeof(int) * dim_matrix * dim_matrix;
  int* c = (int*) malloc(size_matrix);

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
  kernel = clCreateKernel(program, "matrix_mult_tiles", &err);
  if (!kernel || err != CL_SUCCESS)
  {
    printf("Error: Failed to create compute kernel!\n");
    exit(1);
  }

  printf("Successfully created kernel.\n");

  // Create the input and output arrays in device memory for our calculation
  //
  a_mem = clCreateBuffer(context,  CL_MEM_COPY_HOST_PTR,  size_matrix, a, NULL);
  b_mem = clCreateBuffer(context,  CL_MEM_COPY_HOST_PTR,  size_matrix, b, NULL);
  c_mem = clCreateBuffer(context,  CL_MEM_WRITE_ONLY, size_matrix, NULL, NULL);

  if (!a_mem || !b_mem || !c_mem)
  {
      printf("Error: Failed to allocate device memory!\n");
      exit(1);
  }

  printf("Successfully allocated device memory for A, B and result matrix C.\n");

  err = 0;
  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &a_mem);
  err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &b_mem);
  err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &c_mem);
  err |= clSetKernelArg(kernel, 3, sizeof(int), &dim_matrix);

  if (err != CL_SUCCESS)
  {
      printf("Error: Failed to set kernel arguments! %d\n", err);
      exit(1);
  }

  // Execute the kernel over the entire range of our 1d input data set
  // using the maximum number of work group items for this device
  //

  err = clEnqueueNDRangeKernel(commands, kernel, 2, NULL, global_size, local_size, 0, NULL, NULL);
  if (err)
  {
      printf("Error: Failed to execute kernel %d!\n", err);
      return EXIT_FAILURE;
  }

  // Wait for the command commands to get serviced before reading back results
  //
  clFinish(commands);

  printf("Kernel executed successfully, will write back results\n");

  err = clEnqueueReadBuffer(commands, c_mem, CL_TRUE, 0, size_matrix, c, 0, NULL, NULL);

  if (err != CL_SUCCESS)
  {
      printf("Error: Failed to read output array! %d\n", err);
      exit(1);
  }

  end = clock();
  time_used = ((double) end - start) / CLOCKS_PER_SEC * 1000.0f;

  printf("Addition took %f ms.", time_used);
  printf("Will compare the results now\n");

  int* c_expected = dot(a, b, dim_matrix);

  for(int row=0; row < dim_matrix; row++){
    for(int col=0; col < dim_matrix; col++){
      int cur = row * dim_matrix + col;
      if(c[cur] != c_expected[cur]){
        printf("x");
      }
    }
  }

  printf("\nComparison finished \n");

  clReleaseMemObject(a_mem);
  clReleaseMemObject(b_mem);
  clReleaseMemObject(c_mem);

  free(a);
  free(b);
  free(c);
}
