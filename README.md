# Zackernel: an Engine for IoT

Zackernel is a tiny kernel that uses an event-driven, non-blocking I/O model like [Node.js](https://github.com/nodejs/node). It is written in C++11, to use lambda expressions.

Now, Zackernel works on the following environments:

* Arduino

Zackernel needs the following software:

* Download zip file from [Vlpp for Arduino](https://github.com/zackernel/functional-vlpp), and install it Sketch > Include Library > Add .ZIP Library. ([Documents how to install the library](https://www.arduino.cc/en/Guide/Libraries#toc4))
	* Thanks for [Original Vlpp for Arduino](https://github.com/marcusrugger/functional-vlpp) and [Original Vlpp](https://github.com/vczh-libraries/Vlpp) 


## Milestone History

### Code Name "Hiroki": 2016092401

Code Name "Hiroki": Bug fix with developing some applications using LEDs and switches.

The fixed bugs are:

* [Wrong sleep time is caused](https://github.com/zackernel/zackernel/issues/41)
* [Don't Dispatch to a sleeping task when there is a Busy Waiting.](https://github.com/zackernel/zackernel/issues/43)

### Code Name "Kagoshima-Chuo": 2016090901

Code Name "Kagoshima-Chuo": Implementing Basic Loop Functions

Sample Client Code:

```c++
zLoop([&] {
  // calculate something forever with calling other tasks
});

bool flag = true;
zWhile([&] { return flag; }, [&] {
  // calculate something while flag is true with calling other tasks
});

bool flag = true;
zDoWhile([&] {
  // calculate something once and while flag is true with calling other tasks
}, [&] { return flag; });

int i;
zFor([&] { i = 1; }, [&] { i < NUM; }, [&] { i++; }, [&] {
  // calculate something while i < NUM with calling other tasks.
});
```

### Code Name "Kagoshima": 2016090701

Implementing Absolute Time Management.
