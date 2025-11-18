#include <M5Unified.h>
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

const uint8_t extio_address = 0x45;

const uint8_t SETMODE = 0x00;
const uint8_t SERVO_ANGLE_8B_REG = 0x50;

const uint8_t DIGITAL_INPUT_MODE = 0;
const uint8_t DIGITAL_OUTPUT_MODE = 1;
const uint8_t ADC_INPUT_MODE = 2;
const uint8_t SERVO_CTL_MODE = 3;
const uint8_t RGB_LED_MODE = 4;

const int Num_servos = 2;
bool init_ok[Num_servos] = {false, false};
int current_pos[Num_servos];
int target_pos[Num_servos];

#define I2C M5.In_I2C

void
servo_attach(int ch)
{
  int _ch = ch - 1;
  if (ch < 1 || ch > Num_servos || init_ok[_ch]) {
    return;
  }
  M5_LOGD("servo_attach: %d", ch);
  uint8_t mode = SERVO_CTL_MODE;
  if (!I2C.writeRegister(extio_address, SETMODE + _ch, &mode, 1, 400000)) {
    return;
  }
  delay(100);
  init_ok[_ch] = true;
}

void
servo_detach(int ch)
{
  int _ch = ch - 1;
  if (ch < 1 || ch > Num_servos || !init_ok[_ch]) {
    return;
  }
  M5_LOGD("servo_dettach: %d", ch);
  uint8_t mode = DIGITAL_INPUT_MODE;
  if (!I2C.writeRegister(extio_address, SETMODE + _ch, &mode, 1, 400000)) {
    return;
  }
  delay(100);
  init_ok[_ch] = false;
}

void
init_servo()
{
  for (int ch = 1; ch <= Num_servos; ch ++) {
    int _ch = ch - 1;
    init_ok[_ch] = false;
    current_pos[_ch] = target_pos[_ch] = -1;
  }
}

void
servo_set_force(int ch, int degrees)
{
  int _ch = ch - 1;
  if (ch < 1 || ch > Num_servos) {
    return;
  }
  servo_attach(ch);
  if (init_ok[_ch]) {
    M5_LOGD("servo_set_force: %d to %d", ch, degrees);
    uint8_t deg = (uint8_t) degrees;
    I2C.writeRegister(extio_address, SERVO_ANGLE_8B_REG + _ch, &deg, 1, 400000);
    current_pos[_ch] = target_pos[_ch] = (int) deg;
    delay(100);
  }
  servo_detach(ch);
}

void
servo_set(int ch, int degrees)
{
  int _ch = ch - 1;
  if (ch < 1 || ch > Num_servos) {
    return;
  }
  if (current_pos[_ch] < 0) {
    servo_set_force(ch, degrees);
    return;
  }
  M5_LOGD("servo_set: %d to %d", ch, degrees);
  target_pos[_ch] = max(min(degrees,180),0);
}

void servo_update()
{
  for (int ch = 1; ch <= Num_servos; ch ++) {
    int _ch = ch - 1;
    int target = target_pos[_ch];
    int current = current_pos[_ch];
    int step = sign(target - current);
    if (!step) {
      servo_detach(ch);
      continue;
    }
    servo_attach(ch);
    current += step;
    M5_LOGD("servo_update: %d to %d", ch, current);
    uint8_t deg = (uint8_t) current;
    I2C.writeRegister(extio_address, SERVO_ANGLE_8B_REG + _ch, &deg, 1, 400000);
    current_pos[_ch] = current;
  }
}
