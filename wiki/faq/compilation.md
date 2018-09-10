# Compilation Issues

This page summarizes a list of questions related to the compilation and building of OpenTimer.

---

#### Q: What compiler does OpenTimer support now?

**A:** OpenTimer currently requires g++ v7.2 and clang++ v6.0 with -std=c++17.

#### Q: How do I switch to different compilers to compile OpenTimer?

**A:** OpenTimer uses CMake to manage the build process. You can switch to either g++ or clang++
by overriding the variable `CMAKE_CXX_COMPILER`.

```bash
# use clang++ to build OpenTimer
~$ cmake ../ -DCMAKE_CXX_COMPILER=clang++
~$ make

# use g++ to build OpenTimer
~$ cmake ../ -DCMAKE_CXX_COMPILER=g++
~$ make
```

#### Q: How can I clean up the entire project and rebuild the source?

**A:** When cmake gets messed up with different settings, you can remove 
`build/CMakeCache.txt` or remove the entire `build` to start from scratch.

```bash
# remove CMake cached files
~$ make clean
~$ rm CMakeCache.txt
~$ cmake ../
~$ make

# or you can remove the entire build folder and start over
~$ rm -rf build
~$ mkdir build
~$ cd build
~$ cmake ../
~$ make
```

* * *



