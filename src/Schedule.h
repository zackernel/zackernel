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

#ifndef ZCK_SCHEDULE_H
#define ZCK_SCHEDULE_H

#include <functional-vlpp.h>
#include <limits.h>

void null_func();

class Schedule {
  private:
    static Schedule *_queue;
    int _delay_time;
    Schedule *_prev;
    Schedule *_next;
    vl::Func<void(void)> _function;

  public:
    static void init();

    static Schedule *queue();

    static Schedule *first();

    static int is_empty();

    static void add(int delay_time, vl::Func<void(void)> func);

    static Schedule *pull();

    Schedule(int delay_time, vl::Func<void(void)> func);

    void link(Schedule *next);

    int delay_time();

    Schedule *next();

    Schedule *prev();

    void wait();

    void call();

    int is_end();
};

#endif // ZCK_SCHEDULE_H
