#include <M5Unified.h>
#include "config.h"
#include "servo.h"

#ifndef sign
#define sign(x) (((x)==0)?0:((x)>0?1:-1))
#endif
#ifndef min
#define min(x,y) ((x)<(y)?(x):(y))
#endif
#ifndef max
#define max(x,y) ((x)>(y)?(x):(y))
#endif

const int servo_step = (servo_max - servo_min) / 180;
const int pins[] = {servo1Pin, servo2Pin};
static int current_pos[2];
static unsigned long lasts[2];

void
init_servo()
{
  for (int ch = 1; ch <= 2; ch ++) {
    pinMode(pins[ch-1], OUTPUT);
    ledcSetup((uint8_t) ch, PWM_Hz, PWM_level);
    ledcAttachPin(pins[ch-1], ch);
  }
  servo_set_force(1, 25);
  servo_set_force(2, 90);
  lasts[0] = millis();
  lasts[1] = millis();
}

void
servo_set_force(int ch, int degrees)
{
  if (ch < 1 || ch > 2) {
    return;
  }
  degrees = max(min(degrees,180),0);
  ledcWrite(ch, degrees * servo_step + servo_min);
  current_pos[ch - 1] = degrees;
  lasts[ch-1] = millis();
}

void
servo_set(int ch, int degrees)
{
  if (ch < 1 || ch > 2) {
    return;
  }
  if( lasts[ch-1] + 1000 > millis()) {
    return;
  }
  ledcAttachPin(pins[ch-1], ch);
  degrees = max(min(degrees,180),0);
  int pos = current_pos[ch - 1];
  int step = sign(degrees - pos);
	for (; pos != degrees; pos += step) {
    ledcWrite(ch, pos * servo_step + servo_min);
		delay(moving_delay);
	}
  current_pos[ch - 1] = degrees;
  ledcDetachPin(pins[ch-1]);
  lasts[ch-1] = millis();
}
