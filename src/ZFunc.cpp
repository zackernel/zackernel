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

#include <ZFunc.h>

ZFunc ZFunc::_funcPool[POOL];
BitMap ZFunc::_mapPool(POOL, _buf);;
unsigned long ZFunc::_buf[(POOL + 31) / 32];

void ZFunc::init() {
  for (int i = 0; i < POOL; i++) {
    _mapPool.setBit(i, false);
  }
}

void* ZFunc::operator new(size_t size) {
  for (int i = 0; i < POOL; i++) {
    if (!_mapPool.getBit(i)) {
      _mapPool.setBit(i, true);
      return &(_funcPool[i]);
    }
  }
  return NULL;
}

void ZFunc::operator delete(void* p) {
  for (int i = 0; i < POOL; i++) {
    if (p == &(_funcPool[i])) {
      _mapPool.setBit(i, false);
      return;
    }
  }
}

ZFunc::ZFunc() {}

ZFunc::~ZFunc() {}

UFunc::UFunc() {}

UFunc::~UFunc() {}


bool ZFunc::isBool() {
  return _isBool;
}

VFunc ZFunc::vfunc() {
  return _func._vfunc;
}

BFunc ZFunc::bfunc() {
  return _func._bfunc;
}

ZFunc* ZFunc::setVFunc(VFunc vfunc) {
  _isBool = false;
  _func._vfunc = vfunc;
  return this;
}

ZFunc* ZFunc::setBFunc(BFunc bfunc) {
  _isBool = true;
  _func._bfunc = bfunc;
  return this;
}

