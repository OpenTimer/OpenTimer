# OpenTimer <img align="right" width="10%" src="image/logo.png">

[![Build Status](https://travis-ci.org/OpenTimer/OpenTimer.svg?branch=master)](https://travis-ci.org/OpenTimer/OpenTimer)
[![Standard](image/cpp17.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![Download](image/download.svg)](https://github.com/OpenTimer/OpenTimer/archive/master.zip)
[![Version](image/version_badge.svg)](https://github.com/OpenTimer/OpenTimer/tree/master)
[![AskMe](image/askme.svg)][Github issues]
[![Insights](image/maintained.svg)][Github insights]
[![License: MIT](./image/license_badge.svg)](./LICENSE)

A High-Performance Timing Analysis Tool for VLSI Systems

# Why OpenTimer?

<img align="right" width="30%" src="image/critical_path.jpg">

OpenTimer is a new static timing analysis (STA) tool to help IC designers
quickly verify the circuit timing.
It is developed completely from the ground up using *C++17*
to efficiently support parallel and incremental timing. 
Key features are:

+ Industry standard format (.lib, .v, .spef, .sdc) support.
+ Graph- and path-based timing analysis.
+ Parallel incremental timing for fast timing closure.
+ Award-winning tools and golden timers in ACM TAU timing analysis contests.


# Get Started with OpenTimer

The easiest way to start using OpenTimer is to use *OpenTimer shell*.
OpenTimer shell is a powerful tool for interactive analysis
and the simplest way to learn core functionalities.
[Compile OpenTimer](#compile-opentimer) and launch the shell program `ot-shell`
under the `bin` directory.

```bash
~$ ./bin/ot-shell
  ____              _______              
 / __ \___  ___ ___/_  __(_)_ _  ___ ____
/ /_/ / _ \/ -_) _ \/ / / /  ' \/ -_) __/
\____/ .__/\__/_//_/_/ /_/_/_/_/\__/_/       v2.0.0 (alpha)
    /_/                                     
MIT License: type "license" to see more details.
For help, type "help".
For bug reports, issues, and manual, please see:
<https://github.com/OpenTimer/OpenTimer>.
ot> 
```

We have provided a [simple design](./example/simple)
which consists of five gates (one NAND, one NOR, two INV, one FF) and one clock. 
Move to the folder and read this simple design.

<img align="right" src="image/simple_schematic.jpg" width="55%">

```bash
ot> cd example/simple
ot> read_celllib osu018_stdcells.lib
ot> read_verilog simple.v   
ot> read_sdc simple.sdc
```

Report the timing to show the most critical path.

```bash
ot> report_timing                     # report the most critical path
Endpoint     :   f1:D
Startpoint   :   inp1
Path type    :   early
Required Time:   26.5175
Arrival Time :   2.97909
Slack        :   -23.5384
----------------------------------
   Arrival       Delay   Dir   Pin
----------------------------------
         0         n/a  fall  inp1
         0           0  fall  u1:A
   2.79669     2.79669  rise  u1:Y
   2.79669           0  rise  u4:A
   2.97909    0.182399  fall  u4:Y
   2.97909           0  fall  f1:D
----------------------------------
```

The critical path originates from the primary input `inp1` all the way 
to the data pin `f1:D` of the flip-flop `f1`.
<!--You can dump a dot format and use online tools like [GraphViz][GraphViz]
to visualize the timing graph.
The critical path is marked in red.

```bash
ot> dump_graph -o simple.dot  # dump the timing graph to the .dot format
```

![](image/simple_graph.png)

We have provided three command files, 
[simple.conf](./example/simple/simple.conf), 
[unit.conf](./example/simple/unit.conf), and 
[opt.conf](./example/simple/opt.conf)
to demonstrate more usages of OpenTimer. 
You can redirect it through stdin to OpenTimer shell.

```bash
cd example/simple
../../bin/ot-shell < opt.conf     # modify the design and perform incremental timing
```

The graph below is generated after applying gate changing operations in 
[opt.conf](./example/simple/opt.conf).
One gate (marked in cyan) is inserted to the timing graph.
By default, OpenTimer performs parallel incremental timing to maintain slack integrity.

![](image/simple_opt_graph.png) -->

# Compile OpenTimer

## System Requirements

OpenTimer is very self-contained and has very few dependencies.
To compile OpenTimer, you need a [C++17][C++17] compiler. 
We currently support:

+ GNU C++ Compiler G++ v7.2 with -std=c++1z
+ Clang 5.0 C++ Compiler with -std=c++17

In addition, you need a tcl shell interpreter:

+ [tclsh](https://www.tcl.tk/about/language.html) 
(most Unix/Linux/OSX distributions already include tclsh)

OpenTimer has been tested to run well on Linux distributions and MAC OSX.

## Build through CMake

We use [CMake](https://cmake.org/) to manage the source and tests.
We recommend using out-of-source build.

```bash
~$ git clone https://github.com/OpenTimer/OpenTimer.git
~$ cd OpenTimer
~$ mkdir build
~$ cd build
~$ cmake ../
~$ make 
```

After successful build, you can find binaries and libraries in the folders `bin` 
and `lib`, respectively.

## Run Tests
OpenTimer uses [Doctest](https://github.com/onqtam/doctest) for unit tests and
[TAU15][TAU15] benchmarks for integration/regression tests. These benchmarks are generated by an 
industry standard timer and are being used by many EDA researchers.

```bash
~$ make test
```

# Design Philosophy

OpenTimer has very efficient data structures and procedures to
enable parallel and incremental timing.
To make the most use of multi-threading,
each timing operation is divided into three categories,
*Builder*, *Action*, and *Accessor*.

| Type |  Description | Example | Time Complexity |
| -------  |  ----------- |  --- | -- |
| Builder  | create lazy tasks to build an analysis framework | read_celllib, insert_gate, set_slew | O(1) |
| Action   | carry out builder operations to update the timing | update_timing, report_timing, report_slack | Algorithm-dependent |
| Accessors| inspect the timer without changing any internal data structures | dump_timer, dump_slack, dump_net_load | Operation-dependent |

## Builder: OpenTimer Lineage

OpenTimer maintains a lineage graph of *builder* operations 
to create a *task execution plan* (TEP).
A TEP starts with no dependency and keeps adding tasks to the lineage graph
every time you call a builder operation.
It records what transformations need to be executed 
after an action has been called.


![](image/lineage.png)

The above figure shows an example
lineage graph of a sequence of builder operations.
The cyan path is the main lineage line 
with additional tasks attached to enable parallel execution.
OpenTimer use [Cpp-Taskflow][Cpp-Taskflow] to create dependency graphs.

## Action: Update Timing

A TEP is materialized and executed when the timer is requested to perform 
an *action* operation.
Each action operation triggers timing update
from the earliest task to the one that produces the result
of the action call.
Internally, OpenTimer creates task dependency graph to update timing in parallel,
including forward (slew, arrival time) 
and backward (required arrival time) propagations.

![](image/update_timing_simple.png)

The figure above shows the dependency graph (forward in white, backward in cyan) 
to update the timing of the 
[simple](example/simple/simple.v) design.
When an action call finishes, it cleans out the lineage graph
with all timing up-to-date.

## Accessor: Inspect OpenTimer

The *accessor* operations let you inspect the timer status and dump timing information.
All accessor operations are declared as *const methods* in the timer class. 
Calling them promises not to alter any internal members.
For example, you can dump the timing graph into a dot format and use tools like [GraphViz][GraphViz]
for visualization.

<img align="right" src="image/simple_graph_raw.png" width="70%">

```bash
ot> dump_graph  
digraph TimingGraph {
  "inp1";
  ...  # skip for short
  "f1:CLK" -> "f1:Q";
  "u1:B" -> "u1:Y";
  "u1:A" -> "u1:Y";
}
```

# OpenTimer Shell

OpenTimer shell is a powerful command line tool to perform interactive analysis.
It is also the easiest way to get your first timing report off the ground.
The program `ot-shell` can be found in the folder `bin/` after you 
[Compile OpenTimer](#compile-opentimer).

## Commands

The table below shows a list of commonly used commands.

| Command | type | Arguments | Description | Example |
| ------- | ---- | --------- | ----------- | ------- |
| read_celllib | builder | [-early \| -late] file | read the cell library for early and late splits | read_celllib -early mylib_Early.lib |
| read_verilog | builder | file | read the verilog netlist | read_verilog mynet.v |
| read_spef | builder | file | read parasitics in SPEF | read_spef myrc.spef |
| read_sdc | builder | file | read a Synopsys Design Constraint file | read_sdc myrule.sdc |
| update_timing | action | n/a | update the timing | update_timing |
| report_timing | action | n/a | report the timing | report_timing |
| report_path | action | [-num_paths k] | report the top-k critical paths | report_path -num_paths 10 |
| report_tns | action | n/a | report the total negative slack | report_tns |
| report_wns | action | n/a | report the worst negative slack | report_wns |
| dump_graph | accessor | [-o file] | dump the present timing graph to a dot format | dump_graph -o graph.dot |
| dump_timer | accessor | [-o file] | dump the present timer details | dump_timer -o timer.txt |
| dump_slack | accessor | [-o file] | dump the present slack values of all pins | dump_slack -o slack.txt |

To see the full command list, visit [OpenTimer Wiki][OpenTimer Wiki].

# Integrate OpenTimer to your Project

There are a number of ways to develop your project on top of OpenTimer.

## Option 1: Add OpenTimer Subproject

The easiest way to build an OpenTimer application is to include it as a subproject
using CMake [add_subdirectory](https://cmake.org/cmake/help/latest/command/add_subdirectory.html).
Copy OpenTimer to your project root directory and configure your CMakeLists 
[as follows](doc/CMakeLists-Subproject.template):

```CMake
cmake_minimum_required (VERSION 3.9)                  # CMake minimum version
project(app)                                          # your OpenTimer application
add_subdirectory(OpenTimer)                           # add OpenTimer project
include_directories(${PROJECT_SOURCE_DIR}/OpenTimer)  # add OpenTimer include

set(CMAKE_CXX_STANDARD 17)                            # enable c++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)                        # thread library (pthread)

add_executable(app app.cpp)                           # link to your app.cpp
target_link_libraries(app OpenTimer Threads::Threads stdc++fs)
```

## Option 2: Install OpenTimer 

Our project [CMakeLists.txt](CMakeLists.txt) has defined the required files
to install when you hit `make install`.
The installation paths are `<prefix>/include`, `<prefix>/lib`, and `<prefix>/bin`
for the hpp files, library, and executable where `<prefix>` can be configured 
through the cmake variable [CMAKE_INSTALL_PREFIX](https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_PREFIX.html#variable:CMAKE_INSTALL_PREFIX).
The following example installs OpenTimer to `/tmp`.

```bash
~$ cd build/
~$ cmake ../ -DCMAKE_INSTALL_PREFIX=/tmp
~$ make 
~$ make install
~$ cd /tmp            # install OpenTimer to /tmp
~$ ls
bin/  include/  lib/  # OpenTimer headers, libraries, and binaries
```

To build your application on top of the OpenTimer headers and library,
you need `-std=c++1z` and `-lstdc++fs` flags
to enable C++17 standard and [filesystem](https://en.cppreference.com/w/cpp/experimental/fs) libraries.

```bash
~$ g++ app.cpp -std=c++1z -lstdc++fs -O2 -I include -L lib -lOpenTimer -o app.out
~$ ./app.out
```

# OpenTimer C++ API

The class [Timer](ot/timer/timer.hpp) is the main entry you need to 
call OpenTimer in your project. 
The table below summarizes a list of commonly used methods.

| Method | Type | Argument | Return | Description |
| ------ | ---- | -------- | ------ | ----------- |
| read_celllib| builder | path, split | self | read the cell library for early and late splits |
| read_verilog| builder | path | self | read a verilog netlist |
| read_spef   | builder | path | self | read parasitics in SPEF |
| read_sdc    | builder | path | self | read a Synopsys Design Constraint file |
| update_timing | action | n/a | void | update the timing; all timing values are up-to-date upon return |
| tns | action | n/a | optional of float | update the timing and return the total negative slack if exists |
| wns | action | n/a | optional of float | update the timing and return the worst negative slack if exists |
| dump_timer | accessor | n/a | string | dump the present timer details |
| dump_slack | accessor | n/a | string | dump the present slack values of all pins |


*All public methods are thread-safe* as a result of OpenTimer lineage.
The example below shows an OpenTimer application and the use of builder, action, and accessor API.

```cpp
#include <ot/timer/timer.hpp>                // top-level header to include

int main(int argc, char *argv[]) {
  
  ot::Timer timer;                                // create a timer instance (thread-safe)
  
  timer.read_celllib("simple.lib", std::nullopt)  // read the library (O(1) builder)
       .read_verilog("simple.v")                  // read the verilog netlist (O(1) builder)
       .read_spef("simple.spef")                  // read the parasitics (O(1) builder)
       .read_sdc("simple.sdc")                    // read the design constraints (O(1) builder)
       .update_timing();                          // update timing (O(1) builder)

  if(auto tns = timer.tns(); tns) std::cout << "TNS: " << *tns << '\n';  // (O(N) action)
  if(auto wns = timer.wns(); wns) std::cout << "WNS: " << *wns << '\n';  // (O(N) action)

  std::cout << timer.dump_timer();  // dump the timer details (O(1) accessor)
  
  return 0;
}
```

To see the full API list, visit [OpenTimer Wiki][OpenTimer Wiki].

# Examples

The folder [example](./example) contains several examples and is a great place to learn how to use OpenTimer.

| Example |  Description | Library |
| ------- |  ----------- | ---- |
| [simple](./example/simple) | A timing report example on a sequential circuit design. | [OSU Free PDK 45nm][OSUPDK] |
| [unit](./example/unit) | A dimensional analysis example with different time unit conversions. | [OSU Free PDK 45nm][OSUPDK] |
| [sizer](./example/sizer) | A gate-sizing example on a design with six NAND2 cells. | [NanGate 45nm Open Cell Library][NanGate] |
| [optimizer](./example/optimizer) | A timing-driven optimization example plus incremental timing update. | [TAU15 contest library][TAU15] |

The folder [benchmark](./benchmark) contains more designs and they are mainly used for internal regression 
and integration tests.


# Who is Using OpenTimer?

OpenTimer is an award-winning tools. It won ACM TAU Timing Analysis Contests multiple times
([1st place][TAU14] in 2014, [2nd place][TAU15] in 2015, and [1st place][TAU16] in 2016)
and the [special price][LibreCores] from 2016 LibreCores Design Contest.
Many industry and academic people are using OpenTimer in their projects:

- [Golden Timer][TAU19], 2019 ACM TAU Timing Analysis Contest on Timing-driven Optimization
- [Golden Timer][TAU18], 2018 ACM TAU Timing Analysis Contest on Timing Reports from an STA Graph
- [Golden Timer][TAU17], 2017 ACM TAU Timing Analysis Contest on Micro Modeling
- [Golden Timer][TAU16], 2016 ACM TAU Timing Analysis Contest on Micro Modeling
- [Golden Timer][ICCAD15], 2015 ACM/IEEE ICCAD Incremental Timing-driven Placement Contest
- [VSD][VSD], VLSI System Design Corporation
- [OpenDesign Flow Database][OpenDesign], the infrastructure for VLSI design and design automation research

| [<img src="image/tau16.png" width="100px">][TAU16] | [<img src="image/tau17.png" width="100px">][TAU17]| [<img src="image/tau18.png" width="100px">][TAU18] | [<img src="image/tau19.png" width="100px">][TAU19]  | [<img src="image/CAD-contest.png" width="100px">][ICCAD15] | [<img src="image/vsd.jpg" width="100px">][VSD] |
| :---: | :---: | :---: | :---: | :---: | :---: |


Please don't hesitate to [let me know][email me] if I forgot your project! 


# Get Involved

+ Report bugs/issues by submitting a [Github issue][Github issues].
+ Submit contributions using [pull requests][Github pull requests].
+ See documentation and manual in [OpenTimer Wiki][OpenTimer Wiki].
+ Stay tuned with our [project progress][Github projects].
+ Read and cite our [ICCAD][OpenTimerPaper] and [TCAD][UI-TimerPaper] papers.

# Contributors & Acknowledgment
OpenTimer is being actively developed and contributed by the following people:
- [Tsung-Wei Huang][Tsung-Wei Huang] created the OpenTimer project is now the chief architect.
- [Martin Wong][Martin Wong] supported the OpenTimer project through NSF and DARPA funding.
- [Chun-Xun Lin][Chun-Xun Lin] implemented the prompt interface of OpenTimer shell.
- [Kunal Ghosh][Kunal Ghosh] provided a list of practical features to include in OpenTimer.
- [Pei-Yu Lee][Pei-Yu Lee] provided useful incremental timing discussion and helped fix bugs.
- [Tin-Yin Lai][Tin-Yin Lai] discussed micro modeling algorithms and helped fix bugs.
- [Jin Hu][Jin Hu] helped define the timing API and produced the golden benchmarks for integration tests.
- [Myung-Chul Kim][Myung-Chul Kim] helped test OpenTimer through ICCAD CAD contest.
- [George Chen][George Chen] helped defined path report formats and test OpenTimer through TAU contest.
- [Jignesh Shah][Jignesh Shah] contributed to the area and power report commands.
- [Pao-I Chen][Pao-I Chen] helped design the logo of OpenTimer.
- [Leslie Hwang][Leslie Hwang] reviewed the documentation and README.

<!--

| [<img src="image/twhuang.jpg" width="100px">][Tsung-Wei Huang] | [<img src="image/mdfwong.jpg" width="100px">][Martin Wong] | [<img src="image/cxlin.jpg" width="100px">][Chun-Xun Lin] | [<img src="image/kunal_ghosh.jpg" width="100px">][Kunal Ghosh] | [<img src="image/pei_yu_lee.jpg" width="100px">][Pei-Yu Lee] | [<img src="image/jin_hu.jpg" width="100px">][Jin Hu] | [<img src="image/myung_chul_kim.jpg" width="100px">][Myung-Chul Kim] |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| [<img src="image/george_chen.jpg" width="100px">][George Chen]| [<img src="image/poyi.jpg" width="100px">][Pao-I Chen] |
-->

Meanwhile, we appreciate the support from many organizations for our development of OpenTimer.
Please don't hesitate to [let me know][email me] if I forgot you! 

| [<img src="image/uiuc.png" width="100px">][UIUC] | [<img src="image/csl.png" width="100px">][CSL] | [<img src="image/nsf.png" width="100px">][NSF] | [<img src="image/darpa.png" width="100px">][DARPA IDEA] | [<img src="image/vsd.jpg" width="100px">][VSD] |
| :---: | :---: | :---: | :---: | :---: |



# License

<img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">

OpenTimer is licensed under the [MIT License](./LICENSE):

>Copyright &copy; 2018 [Dr. Tsung-Wei Huang][Tsung-Wei Huang] and [Dr. Martin Wong][Martin Wong]
>
>The University of Illinois at Urbana-Champaign, IL, USA
>
>Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
>
>The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
>
>THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

* * *

[Tsung-Wei Huang]:       http://web.engr.illinois.edu/~thuang19/
[Chun-Xun Lin]:          https://github.com/clin99
[Martin Wong]:           https://ece.illinois.edu/directory/profile/mdfwong
[Kunal Ghosh]:           mailto:kunalpghosh@gmail.com
[Jin Hu]:                mailto:jinhu@umich.edu
[Myung-Chul Kim]:        mailto:mckima@us.ibm.com
[Tin-Yin Lai]:           mailto:tinyinlai@gmail.com
[Pei-Yu Lee]:            mailto:palacedeforsaken@gmail.com
[George Chen]:           mailto:george@geochrist.com
[Jignesh Shah]:          mailto:jignesh.shah@intel.com
[Pao-I Chen]:            mailto:poyipenny@gmail.com
[Leslie Hwang]:          mailto:tkdlezz@gmail.com 
[Github releases]:       https://github.com/OpenTimer/OpenTimer/releases
[Github issues]:         https://github.com/OpenTimer/OpenTimer/issues
[Github pull requests]:  https://github.com/OpenTimer/OpenTimer/pulls
[Github projects]:       https://github.com/OpenTimer/OpenTimer/projects
[Github insights]:       https://github.com/OpenTimer/OpenTimer/pulse
[GraphViz]:              https://dreampuf.github.io/GraphvizOnline/
[OpenTimer Wiki]:        ./wiki/Home.md
[OpenTimer-1.0]:         https://web.engr.illinois.edu/~thuang19/software/timer/OpenTimer.html
[OpenTimerCitation]:     https://scholar.google.com/scholar?oi=bibs&hl=en&cites=142282068238605079
[OpenTimerPaper]:        doc/iccad15.pdf
[UI-TimerPaper]:         doc/tcad16.pdf
[email me]:              mailto:twh760812@gmail.com
[Cpp-Taskflow]:          https://github.com/cpp-taskflow/cpp-taskflow
[VSD]:                   https://www.vlsisystemdesign.com/
[ICCAD15]:               http://cad-contest.el.cycu.edu.tw/problem_C/default.html
[TAU14]:                 https://sites.google.com/site/taucontest2014/
[TAU15]:                 https://sites.google.com/site/taucontest2015/
[TAU16]:                 https://sites.google.com/site/taucontest2016/
[TAU17]:                 https://sites.google.com/site/taucontest2017/
[TAU18]:                 https://sites.google.com/view/taucontest2018/
[TAU19]:                 https://sites.google.com/view/tau-contest-2019/home
[LibreCores]:            https://fossi-foundation.org/2016/10/13/designcontest
[OpenDesign]:            https://github.com/jinwookjungs/open_design_flow
[DARPA IDEA]:            https://www.darpa.mil/news-events/2017-09-13
[NSF]:                   https://www.nsf.gov/
[UIUC]:                  https://illinois.edu/
[CSL]:                   https://csl.illinois.edu/
[C++17]:                 https://en.wikipedia.org/wiki/C%2B%2B17
[NanGate]:               http://www.nangate.com/?page_id=22
[OSUPDK]:                https://vlsiarch.ecen.okstate.edu/flow/
  

