# Synopsys Design Constraint

OpenTimer reads Synopsys Design Constraint files (.sdc) to 
impose initial timing conditions on the design.
SDC is a tcl-based format to specify the design intent and surrounding constraints
for synthesis, clocking, timing, power, test, and area.

# Admissible SDC Format

OpenTimer uses open-source SDC parser provided by 
[Synopsys Technology Access Program][Synopsys TAP-in].
Even if OpenTimer can parse a complete SDC file,
it supports a limited set of commands for now.
The table below lists all of them:

| Command | Type | Description |
| -- | -- | -- |
| set_input_delay | timing constraint | sets input delay on an input port relative to a clock signal |
| set_output_delay | timing constraint | sets output delay on an output port relative to a clock signal |
| create_clock | timing constraint | creates a clock and defines its waveform in the current design |
| set_input_transition | system interface | sets a fixed transition time on an input port |
| set_load | system interface | sets the load capacitance value on an output port |

## Example

The example demonstrates a valid sdc format to OpenTimer.

```bash
# create a clock
create_clock -period 50 -name my_clock [get_ports my_clock]

# define input arrival time
set_input_delay 0 -min -rise [get_ports my_clock] -clock my_clock
set_input_delay 25 -min -fall [get_ports my_clock] -clock my_clock
set_input_delay 0 -max -rise [get_ports my_clock] -clock my_clock
set_input_delay 25 -max -fall [get_ports my_clock] -clock my_clock
set_input_delay 0 -min -rise [get_ports input] -clock my_clock
set_input_delay 0 -min -fall [get_ports input] -clock my_clock
set_input_delay 5 -max -rise [get_ports input] -clock my_clock
set_input_delay 5 -max -fall [get_ports input] -clock my_clock

# define input transition time
set_input_transition 10 -min -rise [get_ports input] -clock my_clock
set_input_transition 15 -min -fall [get_ports input] -clock my_clock
set_input_transition 20 -max -rise [get_ports input] -clock my_clock
set_input_transition 25 -max -fall [get_ports input] -clock my_clock
set_input_transition 10 -min -rise [get_ports my_clock] -clock my_clock
set_input_transition 15 -min -fall [get_ports my_clock] -clock my_clock
set_input_transition 10 -max -rise [get_ports my_clock] -clock my_clock
set_input_transition 15 -max -fall [get_ports my_clock] -clock my_clock

# define output required arrival time
set_output_delay -10 -min -rise [get_ports out] -clock my_clock
set_output_delay -10 -min -fall [get_ports out] -clock my_clock
set_output_delay 30 -max -rise [get_ports out] -clock my_clock
set_output_delay 30 -max -fall [get_ports out] -clock my_clock

# define the output load capacitance
set_load -pin_load 4 [get_ports out]
```

The above SDC example defines a clock of period 50 (library time unit) 
on the input port `my_clock` and names it `my_clock`.
The arrival time for input ports `input` and `my_clock` are as follows:

| port | type | early / rise | early / fall | late / rise | late / fall |
| :--: | :--: | :----------: | :----------: | :---------: | :---------: |
| input | slew | 10   | 15            | 20           | 25           |
| input | arrival time | 0   | 0            | 5           | 5           |
| my_clock | slew | 10 | 15 | 10 | 15 | 
| my_clock | arrival_time | 10 | 15 | 20 | 25 | 

The output port `out` is asserted with output delay to specify the 
required arrival time with respect to the clock name `my_clock`.
A positive value indicates a delay value to the left (e.g., setup time) 
of the clock edge at the next cycle,
and a negative value indicates a delay value to the right (e.g., hold time) 
of the clock edge at.

| port | type | early / rise | early / fall | late / rise | late / fall |
| :--: | :--: | :----------: | :----------: | :---------: | :---------: |
| out | required arrival time | 10 | 10 | 50-30=20 | 50-30=20 |

Finally, the `set_load` defines a load capacitance of 4 (library unit) 
on the port `out`.

# Reference

1. [Synopsys Technology Access Program (TAP-in)][Synopsys TAP-in]
2. [Synopsys Design Constraints Basics][SDC-Basics]


* * *

[Synopsys TAP-in]: https://www.synopsys.com/community/interoperability-programs/tap-in.html
[SDC-Basics]: http://www.vlsi-expert.com/2011/02/synopsys-design-constraints-sdc-basics.html

