#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "led1642gw_driver.h"

int main(int argc, char** argv) {
  LED1642GW_Driver driver;
  printf("Initialized LED1642GW driver.\n");

  for (int i = 0; i < 16; ++i) {
    driver.brightness[i] = 0x0000;
  }

  driver.brightness[0] = 0xFFFF;
  driver.brightness[1] = 0x0000;
  driver.brightness[2] = 0xA5A5;
  driver.brightness[3] = 0x5A5A;
  driver.write_all_brightness();

  return 0;
}