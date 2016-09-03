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

#include <functional-vlpp.h>

#include <limits.h>

//#define DEBUG

#define LED1 9
#define LED2 10
#define LED3 11

#define TIC 100

void null_func() {
}

class Schedule {
  private:
    static Schedule *_queue;
    int _delay_time;
    Schedule *_prev;
    Schedule *_next;
    vl::Func<void(void)> _function;

  public:
    static void init() {
      _queue = new Schedule(0, null_func);
      _queue->link(new Schedule(INT_MAX, null_func));
    }

    static Schedule *queue() {
      return _queue;
    }

    Schedule(int delay_time, vl::Func<void(void)> func) {
      this->_delay_time = delay_time;
      this->_function = func;
      this->_prev = NULL;
      this->_next = NULL;
    }

    void link(Schedule *next) {
      this->_next = next;
      next->_prev = this;
    }


    int delay_time() {
      return _delay_time;
    }

    static Schedule *first() {
      return _queue->_next;
    }

    static int is_empty() {
      return _queue->_next->is_end();
    }

    Schedule *next() {
      return _next;
    }

    Schedule *prev() {
      return _prev;
    }

    void wait() {
      delay(this->_delay_time);
    }

    void call() {
      _function();
    }

    static void add(int delay_time, vl::Func<void(void)> func) {
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

    static Schedule *pull() {
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

    int is_end() {
      return (this->_next == NULL);
    }

};

Schedule* Schedule::_queue;

void print_queue() {
  Serial.print("q:");
  for(Schedule *s = Schedule::first(); !(s->is_end()); s = s->next()) {
     Serial.print(s->delay_time());
     Serial.print(','); 
  }
  Serial.print('\n');
}

void dispatch() {
  while(!Schedule::is_empty()) {
    #ifdef DEBUG
    print_queue();
    #endif
    Schedule *s = Schedule::pull();
    s->wait();
    s->call();
    delete s;
  }  
}

void sleep(int time, vl::Func<void(void)> func) {
  Schedule::add(time, func);
  dispatch();
}

void fork(vl::Func<void(void)> func1, vl::Func<void(void)> func2) {
  Schedule::add(0, func1);
  Schedule::add(0, func2);
  dispatch();
}

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  Schedule::init();
  Serial.begin(9600);
}

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

void led2() {
  digitalWrite(LED2, HIGH);
  sleep(TIC * 2, [&] {
     digitalWrite(LED2, LOW);
     sleep(TIC * 2, [&] {});
  });
}

void led3() {
  digitalWrite(LED3, LOW);
  sleep(TIC * 2, [&] {
     digitalWrite(LED3, HIGH);
     sleep(TIC * 2, [&] {});
  });
}

void loop() {
  fork(led1, [&] { fork(led2, led3); });
}