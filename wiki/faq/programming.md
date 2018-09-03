# Programming Questions

This page summarizes a list of questions about writing an application on top of OpenTimer.

---

#### Q: What language does OpenTimer support?

**A:** OpenTimer is written in modern C++ and supports only C++ development. 

#### Q: Is OpenTimer thread-safe?

**A:** Yes, OpenTimer is thread-safe. You can create multiple `timer` objects in a program,
each operating on an unique timing view.
People have been doing this for multi-mode multi-corner (MMMC) analysis.

```cpp
ot::Timer timer1;           # create a timer for one corner
ot::Timer timer2;           # create a timer for another corner
timer1.read_celllib("corner1.lib");
timer2.read_verilog("corner2.lib");
...
timer1.update_timing();     # update the timer for the first corner
timer2.update_timing();     # update the timer for the second corner
```

It is also safe to spawn two threads each on a timer.

```cpp
std::thread t1([&] () {     # thread 1 to operate on one timer
  ot::Timer timer1;
  ...
});

std::thread t2([&] () {     # thread 2 to operate on another timer
  ot::Timer timer2;
  ...
});
```



