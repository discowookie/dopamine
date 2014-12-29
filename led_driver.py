#!/usr/bin/env python

import vcdfile
import random

class LEDDriver(object):
  def __init__(self):
    self.vcd_file = vcdfile.VCDFile()
    self.clk = self.vcd_file.add_signal('wire', 1, 'CLK')
    self.sdi = self.vcd_file.add_signal('wire', 1, 'SDI')
    self.le = self.vcd_file.add_signal('wire', 1, 'LE')

    self.brightness = [0] * 16
    self.clock_period = 1000
    self.time = 0

  def write_all_brightess(self):
    for i, b in enumerate(self.brightness):
      self.write_brightness(b, i == 15)

  def write_brightness(self, data, global_latch=0):
    data = bin(data)[2:].zfill(16)
    for i, d in enumerate(data):
      self.vcd_file.add_event(self.time, self.sdi, int(d))
      if global_latch and i > 10 or not global_latch and i > 12:
        self.vcd_file.add_event(self.time, self.le, 1)
      self.clock()
    self.vcd_file.add_event(self.time, self.le, 0)

  def latch_global(self):
    pass

  def clock(self):
    self.vcd_file.add_event(self.time, self.clk, 0)
    self.time += self.clock_period / 2
    self.vcd_file.add_event(self.time, self.clk, 1)
    self.time += self.clock_period / 2

if __name__ == "__main__":
  led_driver = LEDDriver()

  for i in range(16):
    led_driver.brightness[i] = random.randint(0, 65535)

  led_driver.write_all_brightess()

  # led_driver.write_brightness(766)
  # led_driver.write_brightness(2832, global_latch=1)

  led_driver.clock()
  led_driver.clock()
  led_driver.clock()

  print led_driver.vcd_file

  # for c in range(16):
  #   led_driver.brightness[c] = 100 * c
  # led_driver.write_brightness()
