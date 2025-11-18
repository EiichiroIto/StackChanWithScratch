#include <M5Unified.h>
#include "config.h"
#include "parser.h"

const int max_number = 100;
int numbers[max_number];
int numcount = 0;

void add_number(int v)
{
  if (numcount >= max_number) {
    return;
  }
  numbers[numcount++] = v;
}

void skip_eol(HardwareSerial &serial)
{
  serial.println("skip_eol");
  while (serial.available()) {
    serial.read();
  }
}

void parse(HardwareSerial &serial)
{
  serial.println("parse");
  bool flag = false;
  int v = 0;
  numcount = 0;
  if (serial.read() != '*') {
    skip_eol(serial);
    return;
  }
  while (serial.available()) {
    int c = serial.read();
    if (c >= '0' && c <= '9') {
      flag = true;
      v = v * 10 + c - '0';
    } else if (c == ',') {
      if (flag) {
        add_number(v);
        v = 0;
        flag = false;
      }
    }
  }
  if (flag) {
    add_number(v);
  }
}
