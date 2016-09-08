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

#ifndef ZACKERNEL_H
#define ZACKERNEL_H

#include <ZFunc.h>
#include <Schedule.h>

class Zackernel {
  public:
    static void init(bool isMicros = false);
    static void print(char mark = ' ');
    static void sleep(unsigned long time, VFunc block);
    static void fork(VFunc block1, VFunc block2);
    static void zLoop(VFunc block);
    static void zWhile(BFunc expr, VFunc block);
    static void zDoWhile(VFunc block, BFunc expr);
    static void zFor(VFunc init, BFunc expr, VFunc cont, VFunc block);
    static void dispatch();

  private:
    static Schedule* _queue;
    static bool _isMicros;
    static Schedule* _sleepQ;
    static volatile bool _dispatching;
    static Schedule* _current;

    static Schedule* first();
    static void addLast(Schedule* s);
    static Schedule* dispatchBody();
    static Schedule* zForSub(BFunc expr, VFunc cont, VFunc block);

    friend void Schedule::wakeUpWaiting();
};

void sleep(unsigned long time, VFunc block);
void fork(VFunc block1, VFunc block2);
void zLoop(VFunc block);
void zWhile(BFunc expr, VFunc block);
void zDoWhile(VFunc block, BFunc expr);
void zFor(VFunc init, BFunc expr, VFunc cont, VFunc block);

#endif // ZACKERNEL_H
