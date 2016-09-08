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
#include <Zackernel.h>

Schedule Schedule::_schPool[POOL];
BitMap Schedule::_mapPool(POOL, _buf);;
unsigned long Schedule::_buf[(POOL + 31) / 32];

Schedule::Schedule(ZFunc* function, const char* tag, unsigned long timeToSleep) {
  _prev = NULL;
  _next = NULL;
  _function = function;
  _toFire = NULL;
  _timeToSleep = timeToSleep;
  if (tag == NULL) {
    _tag[0] = '\0';
  } else {
    int i;
    for (i = 0; i < TAG_LENGTH - 1 && tag[i] != '\0'; i++) {
      _tag[i] = tag[i];
    }
    _tag[i] = '\0';
  }
}

Schedule::Schedule() {
  _prev = NULL;
  _next = NULL;
  _function = NULL;
  _toFire = NULL;
  _timeToSleep = 0;
  _tag[0] = '\0';
}

void Schedule::init() {
  ZFunc::init();
  for (int i = 0; i < POOL; i++) {
    _mapPool.setBit(i, false);
  }
}

Schedule::~Schedule() {
  this->unlink();
  delete _function;
}

void Schedule::insertBefore(Schedule* s) {
  if (s == NULL || s->_prev != NULL || s->_next != NULL) {
    // error
    return;
  }
  if (this->_prev != NULL) {
    Schedule* p = this->_prev;
    s->_prev = p;
    p->_next = s;
    this->_prev = s;
    s->_next = this;
  } else { // this->_prev == NULL
    // error
    return;
  }
}

void Schedule::append(Schedule* s) {
  if (s == NULL || s->_prev != NULL || s->_next != NULL || this->_next != NULL) {
    // error
    return;
  }
  this->_next = s;
  s->_prev = this;
}

Schedule* Schedule::unlink() {
  Schedule* p = _prev;
  Schedule* n = _next;
  if (p != NULL) {
    p->_next = n;
  }
  if (n != NULL) {
    n->_prev = p;
  }
  _next = NULL;
  _prev = NULL;
  return this;
}

bool Schedule::hasNext() {
  return _next != NULL;
}

Schedule* Schedule::next() {
  return _next;
}

Schedule* Schedule::prev() {
  return _prev;
}

Schedule* Schedule::toFire() {
  return _toFire;
}

void Schedule::setToFire(Schedule* n) {
  _toFire = n;
}

unsigned long Schedule::timeToSleep() {
  return _timeToSleep;
}

void Schedule::setTimeToSleep(unsigned long time) {
  _timeToSleep = time;
}

void Schedule::setWakeUp(Schedule* waiting) {
  if (_toFire == NULL) {
    _toFire = waiting;
  } else {
    Schedule* p = _toFire;
    while (p->_toFire != NULL) {
      p = p->_toFire;
    }
    p->_toFire = waiting;
  }
}

void Schedule::wakeUpWaiting() {
  if (_toFire != NULL) {
    Schedule* s = _toFire;
    _toFire = NULL;
    Zackernel::addLast(s);
  }
}

void Schedule::print() {
  Serial.print(_tag);
  if (_toFire != NULL) {
    _toFire->print();
  }
}

void Schedule::fire() {
  if (_function->isBool()) {
    if ((_function->bfunc())()) {
      wakeUpWaiting();
    }
  } else { // !_function->isBool()
    (_function->vfunc())();
    wakeUpWaiting();
  }
}

Schedule* Schedule::newVFuncSch(VFunc function, const char* tag, unsigned long timeToSleep) {
  return new Schedule((new ZFunc())->setVFunc(function), tag, timeToSleep);
}

Schedule* Schedule::newBFuncSch(BFunc function, const char* tag, unsigned long timeToSleep) {
  return new Schedule((new ZFunc())->setBFunc(function), tag, timeToSleep);
}

void* Schedule::operator new(size_t size) {
  for (int i = 0; i < POOL; i++) {
    if (!_mapPool.getBit(i)) {
      _mapPool.setBit(i, true);
      return &(_schPool[i]);
    }
  }
  return NULL;
}

void Schedule::operator delete(void* p) {
  for (int i = 0; i < POOL; i++) {
    if (p == &(_schPool[i])) {
      _mapPool.setBit(i, false);
      return;
    }
  }
}
