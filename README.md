# Zackernel: an Engine for IoT

Zackernel is a tiny kernel that uses an event-driven, non-blocking I/O model like [Node.js](https://github.com/nodejs/node). It is written in C++11, to use lambda expressions.

Now, Zackernel works on the following environments:

* Arduino

## Sample code

```c++:blink_3leds.cpp
#include <Zackernel.h>

#define LED1 9    // I/O Port for LED 1
#define LED2 10   // I/O Port for LED 1
#define LED3 11   // I/O Port for LED 1

#define TIC 100   // Unit Time of Blinking (msec)

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  Zackernel::init(); // initializing Zackernel
}

// Blinking LED1 using sleep()
void led1() {
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

// Blinking LED2 using sleep()
void led2() {
  digitalWrite(LED2, HIGH);
  sleep(TIC * 2, [&] {
     digitalWrite(LED2, LOW);
     sleep(TIC * 2, [&] {});
  });
}

// Blinking LED3 using sleep()
void led3() {
  digitalWrite(LED3, LOW);
  sleep(TIC * 2, [&] {
     digitalWrite(LED3, HIGH);
     sleep(TIC * 2, [&] {});
  });
}

// Loop and Fork Tasks for blinking LED1, LED2 and LED3
void loop() {
  fork(led1, [&] { fork(led2, led3); });
}
```

