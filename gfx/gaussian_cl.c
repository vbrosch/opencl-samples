#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../misc/ppm_file.c"
#include <CL/opencl.h>
#include "gfx_misc.c"
#include "../misc/opencl_utils.c"

int main(int argc, char** argv){
  int i_w = 512;
  int i_h = 512;
  int number_of_pixels = i_w * i_h;

  float weights[25];
  calculateWeights(weights);

  cl_ushort3* pixels = readPPM("lena.ppm", &i_w, &i_h);
  printf("Loaded image successfully \n");

  int number_of_workitems = i_w;
  int number_of_workitems_in_workgroup = 8;
  int workgroup_count = (i_w * i_h)/(number_of_workitems_in_workgroup * number_of_workitems_in_workgroup);
  int err;

  int count = number_of_workitems;

  printf("Will launch %dx%d work items in %d work groups (wg size = %dx%d)\n", i_w, i_h, workgroup_count, number_of_workitems_in_workgroup,number_of_workitems_in_workgroup);

  size_t image_size = sizeof(cl_ushort3) * i_w * i_h;
  size_t weights_size = sizeof(float) * 25;

  clock_t start, end;
  double time_used;

  start = clock();

  // get kernel source
  char* kernel_source = read_kernel_file("gaussian_cl.cl");

  cl_device_id device_id;             // compute device id
  cl_context context;                 // compute context
  cl_command_queue commands;          // compute command queue
  cl_program program;                 // compute program
  cl_kernel kernel;                   // compute kernel
  char* device_name[128];

  cl_mem original_mem;
  cl_mem result_mem;
  cl_mem weights_mem;

  size_t global_size[] = {i_w, i_h};
  size_t local_size[] = {number_of_workitems_in_workgroup, number_of_workitems_in_workgroup};

  cl_ushort3* result = (cl_ushort3*) malloc(image_size);

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
  kernel = clCreateKernel(program, "gaussian_kernel", &err);
  if (!kernel || err != CL_SUCCESS)
  {
    printf("Error: Failed to create compute kernel!\n");
    exit(1);
  }

  printf("Successfully created kernel.\n");

  // Create the input and output arrays in device memory for our calculation
  //
  original_mem = clCreateBuffer(context,  CL_MEM_COPY_HOST_PTR,  image_size, pixels, NULL);
  weights_mem = clCreateBuffer(context,  CL_MEM_COPY_HOST_PTR, weights_size, weights, NULL);
  result_mem = clCreateBuffer(context,  CL_MEM_WRITE_ONLY, image_size, NULL, NULL);

  if (!original_mem || !weights_mem || !result_mem)
  {
      printf("Error: Failed to allocate device memory!\n");
      exit(1);
  }

  printf("Successfully allocated device memory for Original, Result and Weights.\n");

  err = 0;
  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &original_mem);
  err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &result_mem);
  err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &weights_mem);
  err |= clSetKernelArg(kernel, 3, sizeof(int), &i_w);
  err |= clSetKernelArg(kernel, 4, sizeof(int), &number_of_pixels);

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
      printf("Error: Failed to execute kernel! %d\n", err);
      return EXIT_FAILURE;
  }

  // Wait for the command commands to get serviced before reading back results
  //
  clFinish(commands);

  printf("Kernel executed successfully, will write back results\n");

  err = clEnqueueReadBuffer(commands, result_mem, CL_TRUE, 0, image_size, result, 0, NULL, NULL);

  if (err != CL_SUCCESS)
  {
      printf("Error: Failed to read output array! %d\n", err);
      exit(1);
  }

  end = clock();
  time_used = ((double) end - start) / CLOCKS_PER_SEC * 1000.0f;

  printf("Operation took %f ms.", time_used);

  writePPM("lena_gaussian_cl.ppm", result, i_w, i_h);

  clReleaseMemObject(result_mem);
  clReleaseMemObject(original_mem);
  clReleaseMemObject(weights_mem);

  free(pixels);
  free(result);
}
