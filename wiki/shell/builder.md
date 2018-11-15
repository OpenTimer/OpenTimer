# Builder Command

A builder command is an operation that creates *lazy* tasks to build up 
a timing analysis framework.
OpenTimer maintains a *lineage* graph of builder operations 
to create a task execution plan (TEP). 
Each time you call a builder command, a task or multiple tasks (for parallelism) are attached 
to the lineage.
The lineage is not materialized until you call an action command.
Each builder command takes O(1) time complexity.

| Command | Description |
| :------ | :---------- |
| [read_celllib](#read_celllib) | reads a liberty format library file |
| [read_verilog](#read_verilog) | reads a gate-level verilog netlist  |
| [read_spef](#read_spef)       | reads a file of net parasitics in SPEF format |
| [read_sdc](#read_sdc)         | reads a Synopsys Design Constraint (sdc) file of version 2.1 |
| [read_timing](#read_timing)   | reads a TAU15 Contest assertion file of timing constraints |
| [set_units](#set_units)       | specifies the units used to compute timing values and report results |
| [set_at](#set_at)             | specifies the arrival time of an input port |
| [set_rat](#set_rat)           | specifies the required arrival time of an output port |
| [set_slew](#set_slew)         | specifies the transition time of an input port |
| [set_load](#set_load)         | specifies the load capacitance of an output port |
| [insert_gate](#insert_gate)   | inserts a gate (instance) to the design |
| [remove_gate](#remove_gate)   | removes a gate (instance) from the design |
| [repower_gate](#repower_gate) | repowers a gate (instance) with a new cell |
| [insert_net](#insert_net)     | inserts an empty net to the design |
| [remove_net](#remove_net)     | removes a net from the design |
| [enable_cppr](#enable_cppr)   | enables common path pessimism removal analysis |
| [disable_cppr](#disable_cppr) | disables common path pessimism removal analysis |


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

---

## read_verilog

The `read_verilog` command reads a gate-level verilog netlist and initializes the circuit graph
from the given libraries.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| filename | required | the name of the gate-level verilog netlist file to read  |

The admissible format of verilog files to OpenTimer is described [here](../io/verilog.md).

---

## read_spef

The `read_spef` command reads a parasitics file of SPEF format.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| filename | required | the name of the SPEF file to read |

The admissible format of SPEF files to OpenTimer is described [here](../io/spef.md).

---

## read_sdc

The `read_sdc` command reads a Synopsys Design Constraint (SDC) file (version 2.1).

| Argument | Type | Description |
| :------- | :--- | :---------- |
| filename | required | the name of the SPEF file to read |

The supporting status of SDC commands is described [here](../io/sdc.md).

---

## read_timing

The `read_timing` command reads a [TAU15 Contest Timing Assertion](TAU15) file 
to assert timing constraints.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| filename | required | the name of the timing assertion file to read |

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

---

## insert_gate

The `insert_gate` command inserts a new gate (instance) upon a cell to the design. 
The newly inserted gate is not connected to other gates or nets.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| gate_name cell_name | required  | the names of the gate and name for the insertion operation |

---

## remove_gate

The `remove_gate` command removes a gate from the design. 
All the connections associated with the gate are removed.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| gate_name | required  | the name of the gate to remove from the design |

---

## repower_gate

The `repower_gate` command repowers a gate with another cell.
The new cell must have the same topology as the old cell.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| gate_name cell_name | required  | the names of the gate and the cell for the repower operation |

---

## insert_net

The `insert_net` command inserts an empty net to the design.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| name | required  | the names of the net to insert into the design |

---

## remove_net

The `remove_net` command removes a net from the design. 
All components in the net will be disconnected.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| name | required  | the names of the net to remove from the design |

---

## enable_cppr

The `enable_cppr` command enables the common path pessimism removal analysis.
This triggers path-based timing update and can increase the runtime.

---

## disable_cppr

The `disable_cppr` command disables the common path pessimism removal analysis
and adopts only graph-based analysis.


* * *

[TAU15]:                 https://sites.google.com/site/taucontest2015/





