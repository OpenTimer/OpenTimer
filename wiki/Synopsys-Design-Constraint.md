# Synopsys Design Constraint

OpenTimer reads Synopsys Design Constraint files (.sdc) to 
impose initial timing conditions on the design.
SDC is a tcl-based format to specify the design intent and surrounding constraints
for synthesis, clocking, timing, power, test, and area.

## Admissible SDC Format

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



## Reference

1. [Synopsys Technology Access Program (TAP-in)][Synopsys TAP-in]
2. [Synopsys Design Constraints Basics][SDC-Basics]


* * *

[Synopsys TAP-in]: https://www.synopsys.com/community/interoperability-programs/tap-in.html
[SDC-Basics]: http://www.vlsi-expert.com/2011/02/synopsys-design-constraints-sdc-basics.html

