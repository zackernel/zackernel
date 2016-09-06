# Zackernel: an Engine for IoT

Zackernel is a tiny kernel that uses an event-driven, non-blocking I/O model like [Node.js](https://github.com/nodejs/node). It is written in C++11, to use lambda expressions.

Now, Zackernel works on the following environments:

* Arduino

Zackernel needs the following software:

* [Vlpp](https://github.com/vczh-libraries/Vlpp) ([for Arduino](https://github.com/marcusrugger/functional-vlpp))

## Sample code

```c++:blink_3leds.cpp
#include <Zackernel.h>

const int LED1 = 9;    // I/O Port of LED 1
const int LED2 = 10;   // I/O Port of LED 2
const int LED3 = 11;   // I/O Port of LED 3

const unsigned long TIC = 100;   // Unit Time of Blinking (msec)

void setup() {
  // setup pinMode of LEDs
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  // initializing Zackernel (unit time is msec)
  Zackernel::init(false);
}

// Loop and Fork Tasks for blinking LED1, LED2 and LED3
void loop() {
  // fork subroutines (blinkLed1, blinkLed2, blinkLed3)
  // these will be running concurrently.
  fork(blinkLed1, [&] { fork(blinkLed2, blinkLed3); });
}

// Blink LED1 with sleeping and awaking
void blinkLed1() {
  digitalWrite(LED1, HIGH);
  sleep(TIC, [&] {
     digitalWrite(LED1, LOW);
     sleep(TIC, [&] {
       digitalWrite(LED1, HIGH);
       sleep(TIC, [&] {
         digitalWrite(LED1, LOW);
         sleep(TIC, [&] {});
       });
     });
  });
}

// Blink LED2 with sleeping and awaking
void blinkLed2() {
  digitalWrite(LED2, HIGH);
  sleep(TIC * 2, [&] {
     digitalWrite(LED2, LOW);
     sleep(TIC * 2, [&] {});
  });
}

// Blink LED3 with sleeping and awaking
void blinkLed3() {
  digitalWrite(LED3, LOW);
  sleep(TIC * 2, [&] {
     digitalWrite(LED3, HIGH);
     sleep(TIC * 2, [&] {});
  });
}
```

