#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "led1642gw_driver.h"

#define USED_CLOCK CLOCK_MONOTONIC // CLOCK_MONOTONIC_RAW if available
#define NANOS 1000000000LL

int main(int argc, char** argv) {
  LED1642GW_Driver driver;
  printf("Initialized LED1642GW driver.\n");

  int num_channels = 48;
  for (unsigned int i = 0; i < num_channels; ++i) {
    if ((i % 16) % 3 == 0)
      driver.brightness[i] = 0xFFFF;
    else
      driver.brightness[i] = 0x0000;
  }

  struct timespec begin, current;
  long long start, elapsed, microseconds;
  /* set up start time data */
  if (clock_gettime(USED_CLOCK, &begin)) {
      /* Oops, getting clock time failed */
      exit(EXIT_FAILURE);
  }
  /* Start time in nanoseconds */
  start = begin.tv_sec*NANOS + begin.tv_nsec;

  unsigned int iterations = 1;
  for (int i = 0; i < iterations; ++i) {
    driver.write_all_brightness();
  }

  /* get elapsed time */
  if (clock_gettime(USED_CLOCK, &current)) {
      /* getting clock time failed, what now? */
      exit(EXIT_FAILURE);
  }
  /* Elapsed time in nanoseconds */
  elapsed = current.tv_sec*NANOS + current.tv_nsec - start;
  microseconds = elapsed / 1000 + (elapsed % 1000 >= 500); // round up halves

  printf("Wrote all brightness values %d times; took %f microseconds each\n",
         iterations, float(microseconds) / iterations);

  return 0;
}