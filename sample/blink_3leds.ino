/*
  Blink 3 LEDs
  Run the following tasks, concurrently and repeatedly:
  * blinkLed1: Turns on the LED1 on for 100ms, then off for 100msec, then on for 100ms, then off for 100msec.
  * blinkLed2: Turns on the LED2 on for 200ms, then off for 200msec.
  * blinkLed3: Turns on the LED3 off for 200ms, then on for 200msec.

  The circuit:
  * LED1: pin 9
  * LED2: pin 10
  * LED3: pin 11

  Created 1 Sep 2016
  By Susumu Yamazaki
  Modified 4 Sep 2016
  By Susumu Yamazaki
*/

#include <Zackernel.h>

const int LED1 = 9;    // I/O Port of LED 1
const int LED2 = 10;   // I/O Port of LED 2
const int LED3 = 11;   // I/O Port of LED 3

const int TIC = 100;   // Unit Time of Blinking (msec)

void setup() {
  // setup pinMode of LEDs
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  // initializing Zackernel
  Zackernel::init();

  // fork subroutines (blinkLed1, blinkLed2, blinkLed3)
  // these will be running concurrently.
  fork(blinkLed1, [&] { fork(blinkLed2, blinkLed3); });
}

void loop() {
  // not use
}

// Blink LED1 with sleeping and awaking
void blinkLed1() {
  zLoop([&] {
    digitalWrite(LED1, HIGH);
    sleep(TIC, [&] {
       digitalWrite(LED1, LOW);
       sleep(TIC, [&] {});
    });
  });
}

// Blink LED2 with sleeping and awaking
void blinkLed2() {
  zLoop([&] {
    digitalWrite(LED2, HIGH);
    sleep(TIC * 2, [&] {
       digitalWrite(LED2, LOW);
       sleep(TIC * 2, [&] {});
    });
  });
}

// Blink LED3 with sleeping and awaking
void blinkLed3() {
  zLoop([&] {
    digitalWrite(LED3, LOW);
    sleep(TIC * 3, [&] {
       digitalWrite(LED3, HIGH);
       sleep(TIC * 3, [&] {});
    });
  });
}

