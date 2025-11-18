#include <M5Unified.h>
#include <ESP32Servo.h>
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

Servo servo1;
Servo servo2;
int servo1_pos;
int servo2_pos;

void init_servo();
void servo_set_force(int ch, int degrees);
void servo_set(int ch, int degrees);

void
init_servo()
{
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	servo1.setPeriodHertz(50);
	servo2.setPeriodHertz(50);
	servo1.attach(servo1Pin, minUs, maxUs);
	servo2.attach(servo2Pin, minUs, maxUs);
  servo_set_force(1, 90);
  servo_set_force(2, 90);
}

void
servo_set_force(int ch, int degrees)
{
  Servo &servo = (ch == 1) ? servo1 : servo2;
  int d = max(min(degrees,180),0);
  servo.write(d);
  if (ch == 1) {
    servo1_pos = d;
  } else {
    servo2_pos = d;
  }
}

void
servo_set(int ch, int degrees)
{
  Servo &servo = (ch == 1) ? servo1 : servo2;
  int pos = (ch == 1) ? servo1_pos : servo2_pos;
  int d = max(min(degrees,180),0);
  int step = sign(d - pos);

  M5.Lcd.printf("Servo%d %d->%d\n", ch, pos, d);

	for (; pos != d; pos += step) {
		servo.write(pos);
		delay(10);
	}
  if (ch == 1) {
    servo1_pos = d;
  } else {
    servo2_pos = d;
  }
}
