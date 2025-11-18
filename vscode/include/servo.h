const int Servo_ch1 = 1;
const int Servo_ch2 = 2;

const int minUs = 1000;
const int maxUs = 2000;

extern void init_servo();
extern void servo_set_force(int ch, int degrees);
extern void servo_set(int ch, int degrees);
