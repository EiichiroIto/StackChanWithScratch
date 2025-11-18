const int Servo_ch1 = 1;
const int Servo_ch2 = 2;

const double PWM_Hz = 50; //PWM周波数
const uint8_t PWM_level = 16; //PWM 16bit
const int servo_min = 3100; //2500; //2300;
const int servo_max = 7420; //8500; //9000;
const int moving_delay = 15;

extern void init_servo();
extern void servo_set_force(int ch, int degrees);
extern void servo_set(int ch, int degrees);
