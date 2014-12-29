#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "led1642gw_driver.h"

#define GPIO0_ADDR        0x44E07000
#define GPIO1_ADDR        0x4804C000
#define GPIO2_ADDR        0x481AC000
#define GPIO3_ADDR        0x481AF000

#define GPIO_SIZE         (GPIO1_ADDR - GPIO0_ADDR)

#define GPIO_OE           0x134
#define GPIO_DATAOUT      0x13C
#define GPIO_DATAIN       0x138

#define USR1              1<<22
#define GPIO1_15          1<<15

int main(int argc, char** argv) {
  int fd = open("/dev/mem", O_RDWR | O_SYNC);

  ulong* gpio1 =  (ulong*) mmap(
    NULL, GPIO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO1_ADDR);

  // Make GPIO1_15 an output by setting its output enable low.
  gpio1[GPIO_OE/4] &= (0xFFFFFFFF ^ GPIO1_15);

  for (int i = 0; i < 5; ++i) {
    // Turn on GPIO1_15
    gpio1[GPIO_DATAOUT/4] |= GPIO1_15;
    sleep(1);

    // Turn off GPIO1_15
    gpio1[GPIO_DATAOUT/4] ^= GPIO1_15;
    sleep(1);
  }

  LED1642GW_Driver driver;
  printf("Initialized LED1642GW driver.\n");

  driver.write_all_brightness();

  return 0;
}