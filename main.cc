#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#include "led1642gw_driver.h"

#define USED_CLOCK CLOCK_MONOTONIC // CLOCK_MONOTONIC_RAW if available
#define NANOS 1000000000LL

int main(int argc, char** argv) {
  int clock_period = 10;
  int num_channels = 16;

  LED1642GW_Driver driver(clock_period, num_channels);
  printf("Initialized LED1642GW driver.\n");

  for (unsigned int i = 0; i < num_channels; ++i) {
    // if ((i % 16) % 3 == 0)
    //   driver.brightness[i] = 0xFFFF;
    // else
    //   driver.brightness[i] = 0x0000;
    if (i < 16)
      driver.brightness_[i] = 0x07FF;
    else
      driver.brightness_[i] = 0x0000;
  }

  // driver.brightness[0] = 0xFFFFF
  // driver.brightness[16] = 0xFFFF;
  // driver.brightness[32] = 0xFFFF;
  
  driver.write_configuration_register(0x807f);
  driver.turn_on_all_outputs();

  struct timespec begin, current;
  long long start, elapsed, microseconds;
  /* set up start time data */
  if (clock_gettime(USED_CLOCK, &begin)) {
      /* Oops, getting clock time failed */
      exit(EXIT_FAILURE);
  }
  /* Start time in nanoseconds */
  start = begin.tv_sec*NANOS + begin.tv_nsec;

  int iterations = 0xFFFF;
  for (int i = 0; i < iterations; ++i) {
    for (int b = 0; b < driver.num_channels_; ++b) {
      driver.brightness_[b] = 0x07FF + 0x07FF * sin((float) i / 15.0);
    }
    
    // printf("Writing all brightnesses, %04x\n", driver.brightness_[0]);

    for (int j = 0; j < 2; j++){
      // driver.write_configuration_register(0x807f);
      // driver.turn_on_all_outputs();
      driver.write_all_brightness();
    }

    // Wait one frame time, about 30 ms.
    driver.run_clock(100000);
  }

  /* get elapsed time */
  if (clock_gettime(USED_CLOCK, &current)) {
      /* getting clock time failed, what now? */
      exit(EXIT_FAILURE);
  }
  /* Elapsed time in nanoseconds */
  elapsed = current.tv_sec*NANOS + current.tv_nsec - start;
  microseconds = elapsed / 1000 + (elapsed % 1000 >= 500); // round up halves

  // printf("Wrote all brightness values %d times; took %f microseconds each\n",
  //        iterations, float(microseconds) / iterations);
  
  printf("Running clock forever...\n");
  driver.run_clock_forever();

  return 0;
}