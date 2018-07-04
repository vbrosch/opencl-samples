#include <stdio.h>

int main(int argc, char** argv){
  printf("This program displays 'Hello World' from different OpenMP Threads.\n");
  printf("Current thread: master\n");

  #pragma omp parallel
  {
    printf("Hello from OpenMP Thread!\n");
  }

  printf("Goodbye from Master Thread\n");
}
