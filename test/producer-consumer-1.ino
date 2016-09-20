/*
  Producer Consumer
  Run the following tasks, concurrently and repeatedly:
  * producer: set the variable led1 to LOW for 500 msec, HIGH for 500 msec.
  * consumer: read the variable led1 and turn on/off LED1 in 10 msec cycle.

  The circuit:
  * LED1: pin 9

  Created 9 Sep 2016
  By Susumu Yamazaki
*/

#include <Zackernel.h>

const int LED1 = 9;    // I/O Port of LED 1

void setup() {
  // setup pinMode of LEDs
  pinMode(LED1, OUTPUT);

  // initializing Zackernel
  Zackernel::init();

  // fork subroutines (blinkLed1, blinkLed2, blinkLed3)
  // these will be running concurrently.
  fork(producer, consumer);
}

void loop() {
  // not use
}

volatile bool led1 = LOW;

void producer() {
  zLoop([&] {
     led1 = LOW;
     sleep(500, [&] {
       led1 = HIGH;
       sleep(500, [&] {});
     });
  });
}

void consumer() {
  zLoop([&] {
    digitalWrite(LED1, led1);
    sleep(10, [&] {});
  });
}
