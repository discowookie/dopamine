#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "led1642gw_driver.h"

int main(int argc, char** argv) {
  LED1642GW_Driver driver;
  printf("Initialized LED1642GW driver.\n");

  driver.write_all_brightness();

  return 0;
}