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

void null_func() {}

static void Schedule::init() {
  _queue = new Schedule(0, null_func);
  _queue->link(new Schedule(INT_MAX, null_func));
}

static Schedule *Schedule::queue() {
  return _queue;
}

Schedule::Schedule(int delay_time, vl::Func<void(void)> func) {
  this->_delay_time = delay_time;
  this->_function = func;
  this->_prev = NULL;
  this->_next = NULL;
}

void Schedule::link(Schedule *next) {
  this->_next = next;
  next->_prev = this;
}


int Schedule::delay_time() {
  return _delay_time;
}

static Schedule *Schedule::first() {
  return _queue->_next;
}

static int Schedule::is_empty() {
  return _queue->_next->is_end();
}

Schedule *Schedule::next() {
  return _next;
}

Schedule *Schedule::prev() {
  return _prev;
}

void Schedule::wait() {
  delay(this->_delay_time);
}

void Schedule::call() {
  _function();
}

static void Schedule::add(int delay_time, vl::Func<void(void)> func) {
  Schedule *s;
  for (s = queue(); delay_time >= s->_delay_time; s = s->_next) {
    delay_time -= s->_delay_time;
  }
  Schedule *n = new Schedule(delay_time, func);
  Schedule *p = s->_prev;
  p->link(n);
  n->link(s);
  if (!s->is_end()) {
    s->_delay_time -= delay_time;
  }
}

static Schedule *Schedule::pull() {
  Schedule *ret_value = queue()->_next;
  if (ret_value->is_end()) {
     return ret_value;
  }
  Schedule *p = ret_value->_prev;
  Schedule *n = ret_value->_next;
  p->link(n);
  ret_value->_next = NULL;
  ret_value->_prev = NULL;
  return ret_value;
}

int Schedule::is_end() {
  return (this->_next == NULL);
}

Schedule* Schedule::_queue;
