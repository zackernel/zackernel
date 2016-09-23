#include <Zackernel.h>

const int LED1 = 9;    // I/O Port of LED 1
const int SW1 = 5;     // I/O Port of switch 1

void setup() {
  // setup pinMode of LEDs
  pinMode(LED1, OUTPUT);
  pinMode(SW1, INPUT);

  Serial.begin(9600);

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
    led1 = digitalRead(SW1);
    Serial.print("producer\n");
    Serial.print(led1);
    Serial.print("\n");
  });
}

void consumer() {
  zLoop([&] {
    digitalWrite(LED1, led1);
    Serial.print("consumer\n");
    sleep(10, [&] {});
  });
}
