#include <M5Unified.h>
#include "config.h"
#include "servo.h"
#include "parser.h"
#include "avatar.h"

void setup()
{
  M5.begin();
  UART.begin(115200);
  UART.println("Initializing ...");

  init_servo();
  init_avatar();

  UART.println("Starting ...");
}

void loop()
{
  if (UART.available()) {
    parse(UART);
    if (numcount == 1) {
      if (numbers[0] == 0) {
        // *0
        avatar_clear();
      }
    } else if (numcount >= 2) {
      if (numbers[0] == 1 || numbers[0] == 2) {
        // *1,10
        // *2,170
        servo_set(numbers[0], numbers[1]);
      } else if (numbers[0] == 3) {
        // *3,72,101,108,108,111
        String text;
        for (int i = 1; i < numcount; i ++) {
          text += (char) numbers[i];
        }
        avatar_say(text.c_str());
      } else if (numbers[0] == 4) {
        // *4,1
        // *4,5
        avatar_expression(numbers[1]);
      }
    }
  } else {
    delay(100);
  }
}
