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

bool Zackernel::_isMicros;
volatile bool Zackernel::_dispatching;
Schedule* Zackernel::_queue;
Schedule* Zackernel::_sleepQ;
Schedule* Zackernel::_current;

void nullFunction() {}

void Zackernel::init(bool isMicros) {
  Schedule::init();
  _isMicros = isMicros;
  _dispatching = false;
  _queue = Schedule::newVFuncSch(nullFunction, "top", 0);
  _queue->append(Schedule::newVFuncSch(nullFunction, "end", ULONG_MAX));
  _sleepQ = Schedule::newVFuncSch(nullFunction, "slTop", 0);
  _sleepQ->append(Schedule::newVFuncSch(nullFunction, "slEnd", ULONG_MAX));
}

void Zackernel::print(char mark) {
  Serial.print(mark);
  Serial.print(':');
  for (Schedule* p = firstOfQueue(); p->hasNext(); p = p->next()) {
    p->print();
    Serial.print(", ");
  }
  Serial.print("sQ:");
  for (Schedule* p = _sleepQ->next(); p->hasNext(); p = p->next()) {
    p->print();
    Serial.print(p->timeToSleep());
    Serial.print(", ");
  }
  Serial.print('\n');
}

Schedule* Zackernel::firstOfQueue() {
  return _queue->next();
}

Schedule* Zackernel::firstOfSleepQ() {
  return _sleepQ->next();
}

Schedule* Zackernel::removeFromQueue() {
  Schedule *p = firstOfQueue();
  p->unlink();
  return p;
}

Schedule* Zackernel::removeFromSleepQ() {
  Schedule *p = firstOfSleepQ();
  p->unlink();
  return p;
}

bool Zackernel::queueHasSome() {
  return firstOfQueue()->hasNext();
}

bool Zackernel::sleepQHasSome() {
  return firstOfSleepQ()->hasNext();
}

unsigned long Zackernel::nextSleepTime() {
  return firstOfSleepQ()->timeToSleep();  
}

void Zackernel::wakeUpFirst() {
  addLast(removeFromSleepQ());
}

void Zackernel::sleepNext() {
  if (_isMicros) {
    delayMicroseconds(nextSleepTime());
  } else {
    delay(nextSleepTime());
  }
}

void Zackernel::addLast(Schedule* s) {
  Schedule* p = firstOfQueue();
  while (p->hasNext()) {
    p = p->next();
  }
  p->insertBefore(s);
}

void Zackernel::dispatch() {
  if (_dispatching) {
    return;
  }
  _dispatching = true;
  while ((_current = dispatchBody()) != NULL) {
    _current->fire();
    delete _current;
  }
  _dispatching = false;
}

Schedule* Zackernel::dispatchBody() {
  if (!queueHasSome() && !sleepQHasSome()) {
    return NULL;
  }
  while (queueHasSome() || sleepQHasSome()) {
    if (queueHasSome()) {
      return removeFromQueue();
    }
    if (sleepQHasSome()) {
      if(nextSleepTime() != 0) {
        sleepNext();        
      }
      wakeUpFirst();
    }
  }
  return NULL;
}

void Zackernel::addNewSleep(Schedule* p, unsigned long timeToSleep, VFunc block) {
  Schedule* s = Schedule::newVFuncSch(block, "s", timeToSleep);
  if (_current != NULL) {
    s->setWakeUp(_current->toFire());
    _current->setToFire(NULL);
  }
  p->insertBefore(s);
  if (p->hasNext()) {
    p->setTimeToSleep(timeToSleep - (p->prev())->timeToSleep());
  }
}

void Zackernel::sleep(unsigned long timeToSleep, VFunc block) {
  Schedule* p = _sleepQ->next();
  if(p->hasNext()) {
    while (p->timeToSleep() <= timeToSleep) {
      timeToSleep -= p->timeToSleep();
      p = p->next();
    }
  }
  addNewSleep(p, timeToSleep, block);
  dispatch();
}

void Zackernel::fork(VFunc block1, VFunc block2) {
  addLast(Schedule::newVFuncSch(block1, "fb1", 0));
  addLast(Schedule::newVFuncSch(block2, "fb2", 0));
  dispatch();
}

void Zackernel::zLoop(VFunc block) {
  Schedule* sBlock = Schedule::newVFuncSch(block, "lb", 0);
  Schedule* sNext = Schedule::newVFuncSch([=] { Zackernel::zLoop(block); }, "ln", 0);
  sBlock->setWakeUp(sNext);
  addLast(sBlock);
  dispatch();
}

void Zackernel::zWhile(BFunc expr, VFunc block) {
  Schedule* sExpr = Schedule::newBFuncSch(expr, "we", 0);
  Schedule* sBlock = Schedule::newVFuncSch(block, "wb", 0);
  Schedule* sNext = Schedule::newVFuncSch([=] { Zackernel::zWhile(expr, block); }, "wn", 0);
  sExpr->setWakeUp(sBlock);
  sBlock->setWakeUp(sNext);
  addLast(sExpr);
  dispatch();
}

void Zackernel::zDoWhile(VFunc block, BFunc expr) {
  Schedule* sBlock = Schedule::newVFuncSch(block, "dwb", 0);
  Schedule* sExpr = Schedule::newBFuncSch(expr, "dwe", 0);
  Schedule* sNext = Schedule::newVFuncSch([=] { Zackernel::zDoWhile(block, expr); }, "dwn", 0);
  sBlock->setWakeUp(sExpr);
  sExpr->setWakeUp(sNext);
  addLast(sBlock);
  dispatch();
}

void Zackernel::zFor(VFunc init, BFunc expr, VFunc cont, VFunc block) {
  Schedule* sInit = Schedule::newVFuncSch(init, "fi", 0);
  sInit->setWakeUp(zForSub(expr, cont, block));
  addLast(sInit);
  dispatch();
}

Schedule* Zackernel::zForSub(BFunc expr, VFunc cont, VFunc block) {
  Schedule* sExpr = Schedule::newBFuncSch(expr, "fe", 0);
  Schedule* sCont = Schedule::newVFuncSch(cont, "fc", 0);
  Schedule* sBlock = Schedule::newVFuncSch(block, "fb", 0);
  Schedule* sNext = Schedule::newVFuncSch([=] {
    Zackernel::addLast(Zackernel::zForSub(expr, cont, block));
    Zackernel::dispatch();
  }, "fn", 0);
  sExpr->setWakeUp(sBlock);
  sBlock->setWakeUp(sCont);
  sCont->setWakeUp(sNext);
  return sExpr;
}

void sleep(unsigned long time, VFunc block) {
  Zackernel::sleep(time, block);
}

void fork(VFunc block1, VFunc block2) {
  Zackernel::fork(block1, block2);
}

void zLoop(VFunc block) {
  Zackernel::zLoop(block);
}

void zWhile(BFunc expr, VFunc block) {
  Zackernel::zWhile(expr, block);
}

void zDoWhile(VFunc block, BFunc expr) {
  Zackernel::zDoWhile(block, expr);
}

void zFor(VFunc init, BFunc expr, VFunc cont, VFunc block) {
  Zackernel::zFor(init, expr, cont, block);
}

