#!/usr/bin/env python

import datetime
import math

class VCDFile(object):
  def __init__(self):
    self.date = datetime.datetime.now()
    self.version = "VCDFile"
    self.comment = ""
    self.timescale = "1ns"
    self.signals = []
    self.data = []

  def add_signal(self, type, width, name, default_value=0):
    id = chr(len(self.signals) + 33)
    signal = dict(type=type, width=width, name=name, id=id,
                  default_value=default_value)
    self.signals.append(signal)
    return signal

  def add_event(self, time, signal, value):
    self.data.append(dict(time=time, signal=signal, value=value))

  def _format_value(self, signal, value):
    if signal['width'] == 1:
      value = "%d" % value
      return "%s%s\n" % (value, signal['id'])
    else:
      value = 'b' + bin(value)[2:].zfill(signal['width'])
      return "%s %s\n" % (value, signal['id'])

  def __str__(self):
    out = "$date\n  %s\n$end\n" % self.date
    out += "$version\n  %s\n$end\n" % self.version
    out += "$comment\n  %s\n$end\n" % self.comment
    out += "$timescale %s $end\n" % self.timescale

    out += "$scope module logic $end\n"
    for sig in self.signals:
      out += "$var %s %d %s %s $end\n" % (sig['type'], sig['width'], sig['id'],
                                          sig['name'])
    
    out += "$upscope $end\n"
    out += "$enddefinitions $end\n"

    # Write out default (time=0) values`
    out += "$dumpvars\n"
    for s in self.signals:
      out += self._format_value(s, s['default_value'])
    out += "$end\n";

    sorted_data = sorted(self.data, key=lambda x: x['time'])

    time = 0
    for d in sorted_data:
      if d['time'] != time:
        time = d['time']
        out += "#%d\n" % time
      out += self._format_value(d['signal'], d['value'])

    return out

if __name__ == '__main__':
  v = VCDFile()

  data = v.add_signal('wire', 8, 'data')
  data_valid = v.add_signal('wire', 1, 'data_valid')

  v.add_event(2000, data_valid, 1)

  wavelength = 500
  for i in range(wavelength*5):
    value = int(100 * (math.sin(math.pi * i/wavelength) + 1.0))
    v.add_event(i * 10, data, value)

  print str(v)