# Command Manual

OpenTimer shell interprets each command in three forms, *builder*, *action*, and *accessor*.
+ a builder command creates *lazy* tasks to build up a timing analysis framework
+ an action command triggers the timing update from the earliest task to the one that produces the result of the action call
+ an accessor command dumps static information without altering the internal data structure of the timer

| Command | Form | Description |
| :------ | :--- | :---------- |
| [read_celllib](#read_celllib) | builder | reads a liberty format library file |
| [read_verilog](#read_verilog) | builder | reads a gate-level verilog netlist  |
| [read_spef](#read_spef)       | builder | reads a file of net parasitics in SPEF |
| [read_sdc](#read_sdc)         | builder | reads a Synopsys Design Constraint (sdc) file of version 2.1 |
| [read_timing](#read_timing)   | builder | reads a TAU15 Contest assertion file of timing constraints |
| [set_units](#set_units)       | builder | specifies the units used to compute timing values and report results |
| [set_at](#set_at)             | builder | specifies the arrival time of an input port |
| [set_rat](#set_rat)           | builder | specifies the required arrival time of an output port |
| [set_slew](#set_slew)         | builder | specifies the transition time of an input port |
| [set_load](#set_load)         | builder | specifies the load capacitance of an output port |
| [insert_gate](#insert_gate)   | builder | inserts a gate (instance) to the design |
| [remove_gate](#remove_gate)   | builder | removes a gate (instance) from the design |
| [repower_gate](#repower_gate) | builder | repowers a gate (instance) with a new cell |
| [insert_net](#insert_net)     | builder | inserts an empty net to the design |
| [remove_net](#remove_net)     | builder | removes a net from the design |
| [connect_pin](#connect_pin)   | builder | connects a pin to a net |
| [disconnect_pin](#disconnect_pin) | builder | disconnects a pin from a net |
| [enable_cppr](#enable_cppr)   | builder | enables common path pessimism removal analysis |
| [disable_cppr](#disable_cppr) | builder | disables common path pessimism removal analysis |
| [update_timing](#update_timing) | action | updates the timer to keep all timing values up-to-date |
| [report_timing](#report_timing) | action | reports the critical paths in the design |
| [report_at](#report_at)         | action | reports the arrival time at a pin |
| [report_slew](#report_slew)     | action | reports the transition time at a pin |
| [report_rat](#report_rat)       | action | reports the required arrival time at a pin |
| [report_slack](#report_slack)   | action | reports the slack at a pin |
| [report_tns](#report_tns)       | action | reports the total negative slack of the design |
| [report_wns](#report_wns)       | action | reports the worst negative slack of the design |
| [report_fep](#report_fep)       | action | reports the total failing endpoints in the design |
| [report_area](#report_area)     | action | reports the aggregate cell areas of the design |
| [report_leakage_power](#report_leakage_power) | action | reports the aggregate cell leakage power of the design |
| [license](#license)       | accessor | shows the license information |
| [version](#version)       | accessor | shows the version of the OpenTimer |
| [dump_timer](#dump_timer) | accessor | dumps the design statistics  |
| [dump_taskflow](#dump_taskflow)| accessor | dumps the current lineage graph to a DOT format file |
| [dump_graph](#dump_graph) | accessor | dumps the current timing graph to a DOT format file |

The above list of commands is committed to be stable.
Other commands found in [shell.hpp](../../ot/shell/shell.hpp) but not listed in the table
may change in the future release.

---

## read_celllib

The `read_celllib` command reads a Liberty format library file. 
In most cases, this is the first command to initialize a timing analysis framework.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| -min     | optional | use library for min delay calculation |
| -max     | optional | use library for max delay calculation |
| filename | required | the name of the liberty file to read  |

By default, OpenTimer use the `filename` for both min and max delay calculations, if none are given.

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## read_verilog

The `read_verilog` command reads a gate-level verilog netlist and initializes the circuit graph
from the given libraries.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| filename | required | the name of the gate-level verilog netlist file to read  |

The admissible format of verilog files to OpenTimer is described [here](../io/verilog.md).

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## read_spef

The `read_spef` command reads a SPEF parasitics file.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| filename | required | the name of the SPEF file to read |

The admissible format of SPEF files to OpenTimer is described [here](../io/spef.md).

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## read_sdc

The `read_sdc` command reads a Synopsys Design Constraint (SDC) file (version 2.1).

| Argument | Type | Description |
| :------- | :--- | :---------- |
| filename | required | the name of the SPEF file to read |

The supporting status of SDC commands is described [here](../io/sdc.md).

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## read_timing

The `read_timing` command reads a [TAU15 Contest Timing Assertion](TAU15) file 
to assert timing constraints.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| filename | required | the name of the timing assertion file to read |

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## set_units

The `set_units` command specifies the units used to compute timing values and 
report results.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| -time unit   | optional | the time unit (s) |
| -capacitance unit | optional | the capacitance unit (F) |
| -resistance unit | optional | the resistance unit (Ohm) |
| -voltage unit | optional | the voltage unit (V) |
| -current unit | optional | the current unit (A) |
| -power unit | optional | the power unit (W) |

The `unit` consists of a *numeric*, a *scale*, and a *type*. 
Each unit is parsed through the following C++ regular expression.

```cpp
std::regex unit_regex("([\\+-]?\\d*\\.?\\d+)\\s*([fpnumkM]?)\\s*(s)", std::regex::icase);
```

For example, you can use the following:

+ set_unit -time 1ps -capacitance -1fF
+ set_unit -time 2ns -capacitance -2pF

By default, OpenTimer applies the unit from the file it parsed first (often library unit definitions)
and converts the later ones to this basis.

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## set_at

The `set_at` command specifies the arrival time of an input port at a given 
min/max split and rise/fall transition.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| -pin name | required  | the name of the input port pin |
| delay     | optional  | the arrival time value |
| -min      | optional  | the minimum arrival time        |
| -max      | optional  | the maximum arrival time       |
| -rise     | optional  | the arrival time at the rising edge |
| -fall     | optional  | the arrival time at the falling edge |

By default, OpenTimer assumes *min* split and *rise* transition. 
If *delay* is not given, OpenTimer unsets the arrival time at the pin.

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## set_rat

The `set_rat` command specifies the required arrival time of an output port at a given 
min/max split and rise/fall transition.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| -pin name | required  | the name of the input port pin |
| delay     | optional  | the required arrival time value |
| -min      | optional  | the minmum required arrival time |
| -max      | optional  | the maximum required arrival time |
| -rise     | optional  | the arrival time at the rising edge |
| -fall     | optional  | the arrival time at the falling edge |

By default, OpenTimer assumes *min* split and *rise* transition. 
If *delay* is not given, OpenTimer unsets the required arrival time at the pin.

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## set_slew

The `set_slew` command specifies the transition time of an input port at a given 
min/max split and rise/fall transition.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| -pin name | required  | the name of the input port pin |
| slew     | optional  | the input transition time value |
| -min      | optional  | the minimum transition time |
| -max      | optional  | the maximum transition time |
| -rise     | optional  | the transition time at the rising edge |
| -fall     | optional  | the transition time at the falling edge |

By default, OpenTimer assumes *min* split and *rise* transition. 
If *slew* is not given, OpenTimer unsets the transition time at the pin.

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## set_load

The `set_load` command specifies the load capacitance of an output port at a given 
min/max split and rise/fall transition.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| -pin name | required  | the name of the input port pin |
| load      | optional  | the load capacitance value |
| -min      | optional  | the minimum load capacitance |
| -max      | optional  | the maximum load capacitance |
| -rise     | optional  | the load capacitance at the rising edge |
| -fall     | optional  | the load capacitance at the falling edge |

By default, OpenTimer assumes *min* split and *rise* transition. 
If *slew* is not given, OpenTimer unsets the transition time at the pin.

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## insert_gate

The `insert_gate` command inserts a new gate (instance) upon a cell to the design. 
The newly inserted gate is not connected to other gates or nets.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| gate_name cell_name | required  | the names of the gate and name for the insertion operation |

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## remove_gate

The `remove_gate` command removes a gate from the design. 
All the connections associated with the gate are removed.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| gate_name | required  | the name of the gate to remove from the design |

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## repower_gate

The `repower_gate` command repowers a gate with another cell.
The new cell must have the same topology as the old cell.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| gate_name cell_name | required  | the names of the gate and the cell for the repower operation |

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## insert_net

The `insert_net` command inserts an empty net to the design.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| name | required  | the names of the net to insert into the design |

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## remove_net

The `remove_net` command removes a net from the design. 
All components in the net will be disconnected.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| name | required  | the names of the net to remove from the design |

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## connect_pin

The `connect_pin` command connects a pin to a net.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| pin_name net_name | required  | the names of the pin and the net to enable this connection |

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## disconnect_pin

The `disconnect_pin` command disconnects a pin from the net it is connected to.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| pin_name | required  | the names of the pin to enable this disconnection |

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## enable_cppr

The `enable_cppr` command enables the common path pessimism removal analysis.
This triggers path-based timing update and can increase the runtime.

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## disable_cppr

The `disable_cppr` command disables the common path pessimism removal analysis
and adopts only graph-based analysis.

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## update_timing

The `update_timing` command updates the timer to keep all timing information
(slew, delay, slack) up-to-date.
This is the bottom-most call of every action command.

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## report_timing

The `report_timing` command reports the timing critical paths in the design.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| -num_paths k | optional  | the number of paths to report |
| -min | optional | the min path |
| -max | optional | the max path |
| -rise | optional | the rising endpoint |
| -fall | optional | the falling endpoint |

If `-num_paths k` is not given, OpenTimer report the most critical path.

If neither `-min` nor `-max` is given, OpenTimer report the critical paths across both splits.

If neither `-rise` nor `-fall` is given, OpenTimer report the critical paths across both transitions.

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## report_at

The `report_at` command reports the arrival time of a pin at a given 
min/max split and rise/fall transition.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| -pin name | required  | the name of a pin to query |
| -min      | optional  | the minimum arrival time |
| -max      | optional  | the maximum arrival time |
| -rise     | optional  | the arrival time at the rising edge |
| -fall     | optional  | the arrival time at the falling edge |

By default, OpenTimer assumes *min* split and *rise* transition. 

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## report_slew

The `report_slew` command reports the transition time of a pin at a given 
min/max split and rise/fall transition.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| -pin name | required  | the name of a pin to query |
| -min      | optional  | the minimum transition time |
| -max      | optional  | the maximum transition time |
| -rise     | optional  | the transition time at the rising edge |
| -fall     | optional  | the transition time at the falling edge |

By default, OpenTimer assumes *min* split and *rise* transition. 

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## report_rat

The `report_rat` command reports the required arrival time of a pin at a given 
min/max split and rise/fall transition.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| -pin name | required  | the name of a pin to query |
| -min      | optional  | the minimum required arrival time time |
| -max      | optional  | the maximum required arrival time time |
| -rise     | optional  | the required arrival time time at the rising edge |
| -fall     | optional  | the required arrival time time at the falling edge |

By default, OpenTimer assumes *min* split and *rise* transition. 

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## report_slack

The `report_slack` command reports the slack of a pin at a given 
min/max split and rise/fall transition.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| -pin name | required  | the name of a pin to query |
| -min      | optional  | the minimum slack time |
| -max      | optional  | the maximum slack time |
| -rise     | optional  | the slack time at the rising edge |
| -fall     | optional  | the slack time at the falling edge |

By default, OpenTimer assumes *min* split and *rise* transition. 

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## report_tns

The `report_tns` command reports the total negative slack 
(the sum of negative slack across all endpoints)
in the design.

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## report_wns

The `report_wns` command reports the worst negative slack in the design.

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## report_fep

The `report_fep` command reports the total failing endpoints in the design.

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## report_area

The `report_area` command reports the aggregate cell areas in the design.

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## report_leakage_power

The `report_leakage_power` command reports the aggregate cell leakage power in the design.

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## license

The `license` command shows the license information of OpenTimer. 
Currently, OpenTimer is under MIT license and is completely free for use.

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## version

The `version` command shows the version of OpenTimer in the form of *major*.*minor*.*patch*.

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## dump_timer

The `dump_timer` command dumps the design statistics. 
The output shows a summary of design size, number of pins, number of nets, units, and so on.

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## dump_taskflow

The `dump_taskflow` command dumps the present lineage graph to a DOT format file.
You can use tools like [GraphViz][GraphViz] to visualize the graph.

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>

---

## dump_graph

The `dump_graph` command dumps the present timing graph into a DOT format file.
You can use tools like [GraphViz][GraphViz] to visualize the graph.

<div align="right"><b><a href="#command-manual">↥ back to top</a></b></div>


* * *

[Cpp-Taskflow]:          https://github.com/cpp-taskflow/cpp-taskflow/
[TAU15]:                 https://sites.google.com/site/taucontest2015/
[GraphViz]:              https://dreampuf.github.io/GraphvizOnline/




