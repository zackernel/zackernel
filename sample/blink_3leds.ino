/*
MIT License

Copyright (c) 2016 Susumu Yamazaki

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
