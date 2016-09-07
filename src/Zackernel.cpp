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
#include <Arduino.h>

void Zackernel::init(bool isMicros) {
  Schedule::init(isMicros);
}

void printQueue() {
  Serial.print("q:");
  for(Schedule *s = Schedule::first(); !(s->isEnd()); s = s->next()) {
     Serial.print(s->delayTime());
     Serial.print(','); 
  }
  Serial.print('\n');
}

void dispatch() {
  while(!Schedule::isEmpty()) {
    Schedule *s = Schedule::pull();
    s->wait();
    s->call();
    delete s;
  }  
}

void sleep(unsigned long time, vl::Func<void(void)> func) {
  Schedule::add(time, func);
  dispatch();
}

void fork(vl::Func<void(void)> func1, vl::Func<void(void)> func2) {
  Schedule::add(0, func1);
  Schedule::add(0, func2);
  dispatch();
}

void zLoop(vl::Func<void(void)> func) {
}

