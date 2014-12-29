#ifndef __LED1642GW_DRIVER_H__
#define __LED1642GW_DRIVER_H__

#include <stdio.h>

class LED1642GW_Driver {
public:
  LED1642GW_Driver()
  : clock_period(1000) {

  }

  ~LED1642GW_Driver() {

  }

  void write_all_brightness() {
    for (int i = 0; i < 16; i++) {
      printf("Writing brightness value %d...\n", i);
      write_brightness(brightness[i], false);
    }
  }

  void write_brightness(int data, bool global_latch) {
    for (int i = 15; i >= 0; i--) {
      printf("  Writing bit %d...\n", i);
    }
  }

  int brightness[16];
  int clock_period;
};

#endif