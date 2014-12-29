#ifndef __LED1642GW_DRIVER_H__
#define __LED1642GW_DRIVER_H__

#include <stdio.h>

#define GPIO0_ADDR        0x44E07000
#define GPIO1_ADDR        0x4804C000
#define GPIO2_ADDR        0x481AC000
#define GPIO3_ADDR        0x481AF000

#define GPIO_SIZE         (GPIO1_ADDR - GPIO0_ADDR)

#define GPIO_OE           0x134
#define GPIO_DATAOUT      0x13C
#define GPIO_DATAIN       0x138

#define USR0              1<<21
#define USR1              1<<22
#define USR2              1<<23
#define USR3              1<<24

#define GPIO1_15          1<<15

// #define SDI               1<<15
// #define CLK               1<<16
// #define LE                1<<17

#define SDI               USR0
#define CLK               USR1
#define LE                USR2

class LED1642GW_Driver {
public:
  LED1642GW_Driver()
  : clock_period(250000) {
    // Turn off the triggers for the USR0-3 LEDs, so we can use them for
    // status.
    system("echo none > /sys/class/leds/beaglebone\\:green\\:usr0/trigger");
    system("echo none > /sys/class/leds/beaglebone\\:green\\:usr1/trigger");
    system("echo none > /sys/class/leds/beaglebone\\:green\\:usr2/trigger");
    system("echo none > /sys/class/leds/beaglebone\\:green\\:usr3/trigger");

    // Open /dev/mem and memory map the GPIO registers.
    mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    gpio1 =  (ulong*) mmap(
      NULL, GPIO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, GPIO1_ADDR);

    // Make the three serial pins outputs by setting their output enables low.
    gpio1[GPIO_OE/4] = 0xFFFFFFFF;
    gpio1[GPIO_OE/4] ^=  SDI;
    gpio1[GPIO_OE/4] ^=  CLK;
    gpio1[GPIO_OE/4] ^=  LE;

    gpio1[GPIO_DATAOUT/4] |= SDI;
    gpio1[GPIO_DATAOUT/4] |= CLK;
    gpio1[GPIO_DATAOUT/4] |= LE;
  }

  ~LED1642GW_Driver() {
    close(mem_fd);
  }

  void write_all_brightness() {
    for (int i = 0; i < 16; i++) {
      printf("Writing brightness value %d...\n", i);
      write_brightness(brightness[i], i == 15);
    }
  }

  void write_brightness(int data, bool global_latch) {
    for (int i = 15; i >= 0; i--) {
      printf("  Writing bit %d, value is %d\n", i, data & (1 << i));

      if (data & (1 << i)) {
        gpio1[GPIO_DATAOUT/4] |= SDI;
      } else {
        gpio1[GPIO_DATAOUT/4] &= (0xFFFFFFFF ^ SDI);
      }

      if ((global_latch && i < 5) || (!global_latch && i < 3)) {
        gpio1[GPIO_DATAOUT/4] |= LE;
      }

      gpio1[GPIO_DATAOUT/4] ^= CLK;
      usleep(clock_period / 2);
      gpio1[GPIO_DATAOUT/4] |= CLK;
      usleep(clock_period / 2);
    }

    gpio1[GPIO_DATAOUT/4] ^= LE;
  }

  int brightness[16];

  int clock_period;

  int mem_fd;
  ulong* gpio1;
};

#endif