#ifndef __LED1642GW_DRIVER_H__
#define __LED1642GW_DRIVER_H__

#include <stdio.h>
#include <time.h>

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
#define SDI_BAR           0xFFFFFFFF ^ SDI
#define CLK               USR1
#define CLK_BAR           0xFFFFFFFF ^ CLK
#define LE                USR2
#define LE_BAR           0xFFFFFFFF ^ LE

class LED1642GW_Driver {
public:
  LED1642GW_Driver()
  : clock_period(2) {
    half_clock.tv_sec = 0;
    half_clock.tv_nsec = clock_period / 2;

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
      write_brightness(brightness[i], i == 15);
    }
  }

  void write_brightness(int data, bool global_latch) {
    // The brightness data latch has to last 3-4 clock periods; the
    // global data latch has to last 5-6 clock periods.
    int le_periods = global_latch ? 5 : 3;

    ulong dataout = gpio1[GPIO_DATAOUT/4];

    // LE should fall at the start of the first bit period.
    dataout &= LE_BAR;

    for (int i = 15; i >= 0; i--) {
      // Set the SDI pin high or low for this bit.
      if (data & (1 << i)) {
        dataout |= SDI;
      } else {
        dataout &= SDI_BAR;
      }

      if (i == le_periods) {
        dataout |= LE;
      }

      // Advance one clock period.
      // TODO(wcraddock): this needs real timing.
      dataout &= CLK_BAR;
      gpio1[GPIO_DATAOUT/4] = dataout;
      // nanosleep(&half_clock, NULL);

      dataout |= CLK;
      gpio1[GPIO_DATAOUT/4] = dataout;
      // nanosleep(&half_clock, NULL);
    }
  }

  int brightness[16];

  int clock_period;
  struct timespec half_clock;

  int mem_fd;
  volatile ulong* gpio1;
};

#endif