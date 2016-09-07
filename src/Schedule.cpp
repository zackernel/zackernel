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

#include <Arduino.h>
#include <Schedule.h>

Schedule* Schedule::_queue;
bool Schedule::_isMicros;
unsigned long Schedule::_prevTime;

void nullFunc() {}

void Schedule::init(bool isMicros) {
  _isMicros = isMicros;
  _prevTime = currentTime();
  _queue = new Schedule(0, nullFunc);
  _queue->link(new Schedule(ULONG_MAX, nullFunc));
}

bool Schedule::isMicros() {
  return _isMicros;
}

unsigned long Schedule::currentTime() {
  return _isMicros ? micros() : millis();
}

Schedule *Schedule::queue() {
  return _queue;
}

Schedule::Schedule(unsigned long delayTime, vl::Func<void(void)> func) {
  this->_delayTime = delayTime;
  this->_function = func;
  this->_prev = NULL;
  this->_next = NULL;
}

void Schedule::link(Schedule *next) {
  this->_next = next;
  next->_prev = this;
}

unsigned long Schedule::delayTime() {
  return _delayTime;
}

Schedule *Schedule::first() {
  return _queue->_next;
}

int Schedule::isEmpty() {
  return _queue->_next->isEnd();
}

Schedule *Schedule::next() {
  return _next;
}

Schedule *Schedule::prev() {
  return _prev;
}

void Schedule::wait() {
  unsigned long current = currentTime();
  long adjustment = current - _prevTime;
  long delayTime = this->_delayTime - adjustment;
  if(delayTime > 0) {
    if(_isMicros) {
      delayMicroseconds(delayTime);
    } else {
      delay(delayTime);
    }
  }
  _prevTime = currentTime();
}

void Schedule::call() {
  _function();
}

void Schedule::add(unsigned long delayTime, vl::Func<void(void)> func) {
  Schedule *s;
  for (s = queue(); delayTime >= s->_delayTime; s = s->_next) {
    delayTime -= s->_delayTime;
  }
  while(s->_delayTime == 0) {
    s = s->_next;
  }

  Schedule *n = new Schedule(delayTime, func);
  Schedule *p = s->_prev;
  p->link(n);
  n->link(s);
  if (!s->isEnd()) {
    s->_delayTime -= delayTime;
  }
}

Schedule *Schedule::pull() {
  Schedule *retValue = queue()->_next;
  if (retValue->isEnd()) {
     return retValue;
  }
  Schedule *p = retValue->_prev;
  Schedule *n = retValue->_next;
  p->link(n);
  retValue->_next = NULL;
  retValue->_prev = NULL;
  return retValue;
}

int Schedule::isEnd() {
  return (this->_next == NULL);
}

