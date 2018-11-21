# Timer

The class [Timer](../../ot/timer/timer.hpp) 
defines a set of core methods for timing analysis under the namespace `ot`.
Each method belongs to either a builder, an action, or an accessor operation.

| Command | Form | Description |
| :------ | :--- | :---------- |
| [read_celllib](#read_celllib) | builder | reads a liberty format library file |
| [read_verilog](#read_verilog) | builder | reads a gate-level verilog netlist  |
| [read_spef](#read_spef)       | builder | reads a file of net parasitics in SPEF format |
| [read_sdc](#read_sdc)         | builder | reads a Synopsys Design Constraint (sdc) file of version 2.1 |
| [read_timing](#read_timing)   | builder | reads a TAU15 Contest assertion file of timing constraints |
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
| [disconnect_pin](#disconnect_pin) | builder | disconnect a pin from the net it connects to |
| [enable_cppr](#enable_cppr)   | builder | enables common path pessimism removal analysis |
| [disable_cppr](#disable_cppr) | builder | disables common path pessimism removal analysis |
| [update_timing](#update_timing) | action | updates the timer to keep all timing values up-to-date |
| [report_timing](#report_timing) | action | report the critical paths of the design |
| [report_at](#report_at)       | action | reports the arrival time at a pin |
| [report_slew](#report_slew)   | action | reports the transition time at a pin |
| [report_rat](#report_rat)     | action | reports the required arrival time at a pin |
| [report_slack](#report_slack) | action | reports the slack at a pin |
| [report_tns](#report_tns)     | action | reports the total negative slack of the design |
| [report_wns](#report_wns)     | action | reports the worst negative slack of the design |
| [report_fep](#report_fep)     | action | reports the total failing endpoints in the design |
| [dump_graph](#dump_graph)     | accessor | dumps the timing graph to an output stream |
| [dump_taskflow](#dump_taskflow) | accessor | dumps the lineage graph to an output stream |
| [dump_timer](#dump_timer)     | accessor | dumps the statistics of the design |
| [num_primary_inputs](#num_primary_inputs)| accessor | queries the number of primary inputs in the design |
| [num_primary_outputs](#num_primary_outputs)| accessor | queries the number of primary outputs in the design |
| [num_pins](#num_pins)| accessor | queries the number of pins in the design |
| [num_nets](#num_nets)| accessor | queries the number of nets in the design |
| [num_arcs](#num_arcs)| accessor | queries the number of arcs in the timing graph |
| [num_gates](#num_gates)| accessor | queries the number of gates in the design |
| [num_tests](#num_tests)| accessor | queries the number of timing tests (checks) in the design |
| [primary_inputs](#primary_inputs)| accessor | acquires a const reference to the data structure of primary inputs |
| [primary_outputs](#primary_outputs)| accessor | acquires a const reference to the data structure of primary outputs |
| [pins](#pins) | accessor | acquires a const reference to the data structure of pins |
| [nets](#nets) | accessor | acquires a const reference to the data structure of nets |
| [gates](#gates) | accessor | acquires a const reference to the data structure of gates |
| [clocks](#clocks)| accessor | acquires a const reference to the data structure of clocks |
| [tests](#tests) | accessor | acquires a const reference to the data structure of tests |
| [arcs](#arcs)| accessor | acquires a const reference to the data structure of arcs |

The above list of methods is consider stable.
Other methods found in [timer.hpp](../../ot/timer/timer.hpp) but not listed in the table
may change in the future release.

---

## read_celllib

Reads a Liberty format library file. 
In most cases, this is the first method to initialize a timing analysis framework.

```cpp
Timer& read_celllib(std::filesystem::path path, std::optional<Split> split = {});
```

#### Parameters

+ path: the file path of the liberty file to read
+ split: use this library for either min  or max delay calculation

#### Return Value

`*this`

#### Notes

The method adds two tasks to the lineage graph;
one parses the library and the other attaches the library to the timer.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## read_verilog

Reads a gate-level verilog netlist and initializes the circuit graph from the library.

```cpp
Timer& read_verilog(std::filesystem::path path);
```

#### Parameters

+ path: the file path of the verilog netlist to read

#### Return Value

`*this`

#### Notes

The method adds two tasks to the lineage graph;
one parses the verilog file and the other initializes the circuit graph from it.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## read_spef

Reads a parasitics file in SPEF.

```cpp
Timer& read_spef(std::filesystem::path path);
```

#### Parameters

+ path: the path of the SPEF file to read

#### Return Value

`*this`

#### Notes

The method adds two tasks to the lineage graph;
one parses the SPEF file and the other digests the net parasitics.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## read_sdc

Reads a Synopsys Design Constraint (SDC) file.

```cpp
Timer& read_sdc(std::filesystem::path path);
```

#### Parameters

+ path: the path of the sdc file to read

#### Return Value

`*this`

#### Notes

The method adds two tasks to the lineage graph;
one parses the sdc file and the other digests the timing constraints.

The supporting status of SDC commands is described [here](../io/sdc.md).


<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## read_timing

Reads a [TAU15 Contest Format][TAU15] timing assertion file.

```cpp
Timer& read_timing(std::filesystem::path path);
```

#### Parameters

+ path: the path of the timing assertion file to read

#### Return Value

`*this`

#### Notes

The method adds two tasks to the lineage graph;
one parses the timing file and the other digests the timing constraints.


<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## set_at

Specifies the arrival time of an input port at a given 
min/max split and rise/fall transition.

```cpp
Timer& set_at(std::string port, Split split, Tran tran, std::optional<float> value);
```

#### Parameters

+ port: the name of the input port
+ split: the minimum  or maximum arrival time
+ tran: the rising or falling edge
+ value: the arrival time value; use `std::nullopt` to unset the arrival time of the port

#### Return Value

`*this`

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## set_rat

Specifies the required arrival time of an output port at a given 
min/max split and rise/fall transition.

```cpp
Timer& set_rat(std::string port, Split split, Tran tran, std::optional<float> value);
```

#### Parameters

+ port: the name of the output port
+ split: the required arrival time at min  or max split
+ tran: the rising or falling edge
+ value: the arrival time value; use `std::nullopt` to unset the arrival time of the port

#### Return Value

`*this`

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## set_slew

Specifies the transition time of an input port at a given 
min/max split and rise/fall transition.

```cpp
Timer& set_at(std::string port, Split split, Tran tran, std::optional<float> value);
```

#### Parameters

+ port: the name of the input port
+ split: the minimum  or maximum transition time
+ tran: the rising or falling edge
+ value: the transition time value; use `std::nullopt` to unset the transition time of the port

#### Return Value

`*this`

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## set_load

Specifies the load capacitance of an output port at a given 
min/max split and rise/fall transition.

```cpp
Timer& set_load(std::string port, Split split, Tran tran, std::optional<float> value);
```

#### Parameters

+ port: the name of the output port
+ split: the load capacitance at min  or max split
+ tran: the load capacitance at rising or falling edge
+ value: the load capacitance value; use `std::nullopt` to unset the load capacitance of the port

#### Return Value

`*this`

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## insert_gate

Inserts a gate into the design. 
The newly inserted gate is not connected to any other gates or wires.

```cpp    
Timer& insert_gate(std::string gate, std::string cell);
```

#### Parameters

+ gate: the name of the gate to insert to the design
+ cell: the name of the cell to associate with the gate

#### Return Value

`*this`

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## remove_gate

Removes a gate from the design. 
The gate will be disconnected from the design before removal.

```cpp
Timer& remove_gate(std::string gate);
```

#### Parameters

+ gate: the name of the gate to remove from the design

#### Return Value

`*this`

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## repower_gate

Changes the size or the level of an existing gate, or insert a new gate if it does not exist.

```cpp
Timer& repower_gate(std::string gate, std::string cell);
```

#### Parameters

+ gate: the name of the gate to repower
+ cell: the name of the cell to associate with the gate

#### Return Value

`*this`

#### Notes

The topology of the new cell must be the same as the old cell, or it can result in undefined behavior.
However the pin capacitance, for example, of the new cell can be different.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## insert_net

Inserts an empty net object to the design.

```cpp
Timer& insert_net(std::string net);
```

#### Parameters

+ net: the name of the net to insert to the design

#### Return Value

`*this`

#### Notes

The net will be connected to existing pins in the design by the method `connect_pin`.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## remove_net

Removes a net from the design. 

```cpp
Timer& remove_net(std::string net);
```

#### Parameters

+ net: the name of the net to remove from the design

#### Return Value

`*this`

#### Note

If a net is connected to pins, the pins will be automatically disconnected from the net.
The corresponding parasitics will also be removed.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## connect_pin

Connects a pin to a net.

```cpp
Timer& connect_pin(std::string pin, std::string net);
```

#### Parameters

+ pin: the name of the pin
+ net: the name of the net

#### Return Value

`*this`

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## disconnect_pin

Disconnects a pin from the net it connects to.

```cpp
Timer& disconnect_pin(std::string pin);
```

#### Parameters

+ pin: the name of the pin

#### Return Value

`*this`

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## enable_cppr

Enables the common path pessimism removal (CPPR) analysis.

```cpp
Timer& enable_cppr();
```

#### Parameters

none

#### Return Value

`*this`

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## disable_cppr

Disables the common path pessimism removal (CPPR) analysis.

```cpp
Timer& disable_cppr();
```

#### Parameters

none

#### Return Value

`*this`

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## update_timing

Triggers the timing update to keep all timing information 
(arrival time, slew, required arrival time, etc)
up-to-date.

```cpp
void update_timing();
```

#### Paramters

none

#### Return Value

none

#### Notes

This is the bottom-most call of all action methods.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## report_timing

Reports the critical paths of the design.

```cpp
1. std::vector<Path> report_timing(size_t k);
2. std::vector<Path> report_timing(size_t k, Split split, Tran tran)
3. std::vector<Path> report_timing(size_t k, Split split)
4. std::vector<Path> report_timing(size_t k, Tran tran)
```

#### Paramters

1. reports the top-k critical paths across all endpoints
  + K: the number of paths to report
2. reports the top-k critical min/max paths for rising/falling endpoints
  + K: the number of paths to report
  + split: minimum or maximum path
  + tran: rising or falling endpoints
3. report the top-k critical min/max paths across both rising and falling endpoints
  + K: the number of paths to report
  + split: minimum or maximum path
4. report the top-k critical paths for rising/falling endpoints
  + K: the number of paths to report
  + tran: rising or falling endpoints

#### Return Value

A vector of paths of type `Path`, defined in [path.hpp][ot::Path].


<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>


---

## report_at

Reports the arrival time of a pin at a given 
min/max split and rise/fall edge.

```cpp
std::optional<float> report_at(const std::string& pin, Split split, Tran tran);
```

#### Parameters

+ pin: the name of the pin to report
+ split: the minimum  or maximum arrival time
+ tran: the rising or falling edge

#### Return Value

Returns the arrival time if found, or `std::nullopt`.

#### Notes

The arrival time may not be found, for example, 
the pin doesn't exist or the timing propagation doesn't go through the pin.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## report_rat

Reports the required arrival time of a pin at a given 
min/max split and rise/fall edge.

```cpp
std::optional<float> report_rat(const std::string& pin, Split split, Tran tran);
```

#### Parameters

+ pin: the name of the pin to report
+ split: the minimum  or maximum required arrival time
+ tran: the rising or falling edge

#### Return Value

Returns the required arrival time if found, or `std::nullopt`.

#### Notes

The required arrival time may not be found, for example, 
the pin doesn't exist or the timing propagation doesn't go through the pin.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## report_slew

Reports the transition time of a pin at a given 
min/max split and rise/fall edge.

```cpp
std::optional<float> report_slew(const std::string& pin, Split split, Tran tran);
```

#### Parameters

+ pin: the name of the pin to report
+ split: the minimum  or maximum transition time
+ tran: the rising or falling edge

#### Return Value

Returns the transition time if found, or `std::nullopt`.

#### Notes

The transition time may not be found, for example, 
the pin doesn't exist or the timing propagation doesn't go through the pin.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## report_load

Reports the load capacitance of a net at a given 
min/max split and rise/fall edge.

```cpp
std::optional<float> report_load(const std::string& pin, Split split, Tran tran);
```

#### Parameters

+ pin: the name of the pin to report
+ split: the minimum  or maximum load capacitance
+ tran: the rising or falling edge

#### Return Value

Returns the load capacitance if found, or `std::nullopt`.

#### Notes

The load capacitance may not be found, for example, 
the net doesn't exist or the timing propagation doesn't go through the net.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## report_tns

Reports the total negative slack (TNS) across all endpoints.

```cpp
std::optional<float> report_tns();
```

#### Parameters

none

#### Return Value

Returns the total negative slack if found, or `std::nullopt`.

#### Notes

The total negative slack may not be found, for example, 
the timing propagation doesn't go through any endpoints.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## report_wns

Reports the worst negative slack (WNS) across all endpoints.

```cpp
std::optional<float> report_wns();
```

#### Parameters

none

#### Return Value

Returns the worst negative slack if found, or `std::nullopt`.

#### Notes

The worst negative slack may not be found, for example, 
the timing propagation doesn't go through any endpoints.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## report_fep

Reports the total number of failing endpoints (FEP) across all endpoints.

```cpp
std::optional<size_t> report_fep();
```

#### Parameters

none

#### Return Value

Returns the total number of failing endpoints if found, or `std::nullopt`.

#### Notes

The total number of failing endpoints may be be found, for example, 
the timing propagation doesn't go through any endpoints.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## dump_graph

Dumps the timing graph to a DOT format.

```cpp
void dump_graph(std::ostream& ostream) const;
```

#### Parameters

+ ostream: the target output stream to dump

#### Return Value

none

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## dump_taskflow

Dumps the lineage graph to a DOT format.

```cpp
void dump_taskflow(std::ostream& ostream) const;
```

#### Parameters

+ ostream: the target output stream to dump

#### Return Value

none

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## dump_timer

Dumps the statistics of the design.

```cpp
void dump_timer(std::ostream& ostream) const;
```

#### Parameters

+ ostream: the target output stream to dump

#### Return Value

none

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## num_primary_inputs

Queries the number of primary inputs in the design.

```cpp
size_t num_primary_inputs() const;
```

#### Parameters

none

#### Return Value

An unsigned integer for the number of primary inputs in the design.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## num_primary_outputs

Queries the number of primary outputs in the design.

```cpp
size_t num_primary_outputs() const;
```

#### Parameters

none

#### Return Value

An unsigned integer for the number of primary outputs in the design.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## num_pins

Queries the number of pins in the design.

```cpp
size_t num_pins() const;
```

#### Parameters

none

#### Return Value

An unsigned integer for the number of pins in the design.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## num_nets

Queries the number of nets in the design.

```cpp
size_t num_nets() const;
```

#### Parameters

none

#### Return Value

An unsigned integer for the number of nets in the design.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## num_arcs

Queries the number of arcs in the design.

```cpp
size_t num_arcs() const;
```

#### Parameters

none

#### Return Value

An unsigned integer for the number of arcs in the design.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## num_gates

Queries the number of gates in the design.

```cpp
size_t num_gates() const;
```

#### Parameters

none

#### Return Value

An unsigned integer for the number of gates in the design.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## num_tests

Queries the number of tests (checks) in the design.

```cpp
size_t num_tests() const;
```

#### Parameters

none

#### Return Value

An unsigned integer for the number of tests in the design.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## primary_inputs

Acquires a const reference to the data structure of primary inputs.

```cpp
const std::unordered_map<std::string, Pin>& primary_inputs() const;
```

#### Parameters

none

#### Return Value

A constant reference to the data structure of primary inputs.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## primary_outputs

Acquires a const reference to the data structure of primary outputs.

```cpp
const std::unordered_map<std::string, Pin>& primary_outputs() const;
```

#### Parameters

none

#### Return Value

A constant reference to the data structure of primary outputs.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## pins

Acquires a const reference to the data structure of pins.

```cpp
const std::unordered_map<std::string, Pin>& pins() const;
```

#### Parameters

none

#### Return Value

A constant reference to the data structure of pins.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## nets

Acquires a const reference to the data structure of nets.

```cpp
const std::unordered_map<std::string, Net>& nets() const;
```

#### Parameters

none

#### Return Value

A constant reference to the data structure of nets.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## gates

Acquires a const reference to the data structure of gates.

```cpp
const std::unordered_map<std::string, Gate>& gates() const;
```

#### Parameters

none

#### Return Value

A constant reference to the data structure of gates.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## clocks

Acquires a const reference to the data structure of clocks.

```cpp
const std::unordered_map<std::string, Clock>& clocks() const;
```

#### Parameters

none

#### Return Value

A constant reference to the data structure of clocks.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## tests

Acquires a const reference to the data structure of tests (timing checks).

```cpp
const std::list<Test>& tests() const;
```

#### Parameters

none

#### Return Value

A constant reference to the data structure of tests.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>

---

## arcs

Acquires a const reference to the data structure of arcs.

```cpp
const std::list<Arc>& arcs() const;
```

#### Parameters

none

#### Return Value

A constant reference to the data structure of arcs.

<div align="right"><b><a href="#Timer">↥ back to top</a></b></div>


* * *

[TAU15]:                 https://sites.google.com/site/taucontest2015/
[ot::Path]:              ../../ot/timer/path.hpp


