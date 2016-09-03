# Zackernel: an Engine for IoT

Zackernel is a tiny kernel that uses an event-driven, non-blocking I/O model like [Node.js](https://github.com/nodejs/node). It is written in C++11, to use lambda expressions.

Now, Zackernel works on the following environments:

* Arduino

## Sample code

```c++:blink_3leds.cpp
#include <Zackernel.h>

#define LED1 9    // I/O Port of LED 1
#define LED2 10   // I/O Port of LED 2
#define LED3 11   // I/O Port of LED 3

#define TIC 100   // Unit Time of Blinking (msec)

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  Zackernel::init(); // initializing Zackernel
}

// Blink LED1 using sleep()
void blink_led1() {
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

// Blink LED2 using sleep()
void blink_led2() {
  digitalWrite(LED2, HIGH);
  sleep(TIC * 2, [&] {
     digitalWrite(LED2, LOW);
     sleep(TIC * 2, [&] {});
  });
}

// Blink LED3 using sleep()
void blink_led3() {
  digitalWrite(LED3, LOW);
  sleep(TIC * 2, [&] {
     digitalWrite(LED3, HIGH);
     sleep(TIC * 2, [&] {});
  });
}

// Loop and Fork Tasks for blinking LED1, LED2 and LED3
void loop() {
  fork(blink_led1, [&] { fork(blink_led2, blink_led3); });
}
```

