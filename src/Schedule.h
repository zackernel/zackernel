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

#ifndef TAG_LENGTH
#define TAG_LENGTH 8
#endif

#include <BitMap.h>
#include <ZFunc.h>
#include <limits.h>

class Schedule {
  public:
    void insertBefore(Schedule* s);
    void append(Schedule* s);
    Schedule* unlink();
    bool hasNext();
    Schedule* next();
    Schedule* prev();
    Schedule* toFire();
    unsigned long timeToSleep();
    void setTimeToSleep(unsigned long time);
    void setWakeUp(Schedule* waiting);
    void wakeUpWaiting();
    void setToFire(Schedule* t);
    void fire();
    void* operator new(size_t size);
    void operator delete(void* p);

    void print();
    ~Schedule();

    static Schedule* newVFuncSch(VFunc function, const char* tag, unsigned long timeToSleep);
    static Schedule* newBFuncSch(BFunc function, const char* tag, unsigned long timeToSleep);
    static void init();

  private:
    Schedule();
    Schedule(ZFunc* function, const char* tag, unsigned long timeToSleep = 0);
    Schedule* _prev;
    Schedule* _next;
    Schedule* _toFire;
    ZFunc* _function;
    unsigned long _timeToSleep;
    char _tag[TAG_LENGTH];

    static Schedule _schPool[];
    static unsigned long _buf[];
    static BitMap _mapPool;
};


#endif // ZCK_SCHEDULE_H
