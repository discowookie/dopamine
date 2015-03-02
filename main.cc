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
    driver.brightness_[i] = 0x0000;
  }

  driver.write_configuration_register(0x803f);
  driver.turn_on_all_outputs();

  struct timespec begin, current;
  long long start, elapsed, microseconds;
  if (clock_gettime(USED_CLOCK, &begin)) {
      exit(EXIT_FAILURE);
  }
  start = begin.tv_sec*NANOS + begin.tv_nsec;

  const float red_period = float(rand() % 15);
  const float red_offset = float(rand() % 32767) / 32767.0;
  const float green_period = float(rand() % 15);
  const float green_offset = float(rand() % 32767) / 32767.0;
  const float blue_period = float(rand() % 15);
  const float blue_offset = float(rand() % 32767) / 32767.0;

  const int red_channel = 15;
  const int green_channel = 14;
  const int blue_channel = 13;
  const int midscale = 0x07FF;

  int iterations = 0xFFFF;
  for (int i = 0; i < iterations; ++i) {
    driver.brightness_[red_channel] =
      midscale + midscale * sin((float) i / red_period + red_offset);
    driver.brightness_[green_channel] =
      midscale + midscale * sin((float) i / green_period + green_offset);
    driver.brightness_[blue_channel] =
      midscale + midscale * sin((float) i / blue_period + blue_offset);
    
    for (int j = 0; j < 2; j++){
      driver.write_all_brightness();
    }

    // Wait one frame time, about 30 ms.
    driver.run_clock(100000);
  }

  if (clock_gettime(USED_CLOCK, &current)) {
      exit(EXIT_FAILURE);
  }
  elapsed = current.tv_sec*NANOS + current.tv_nsec - start;
  microseconds = elapsed / 1000 + (elapsed % 1000 >= 500); // round up halves
  
  printf("Running clock forever...\n");
  driver.run_clock_forever();

  return 0;
}