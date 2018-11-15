# Verilog Netlist

OpenTimer reads *gate-level* (aka structural) verilog files (.v) 
to initialize circuit netlists.
Logics are described by gates and modules only. There are no `always` blocks
or `assign` statements.

# Admissible Verilog Format

The input verilog must specify the top-level hierarchy of the design.
For now, OpenTimer uses a small set of keywords in the Verilog language.

```verilog
module circuit_name (       // begin module definition
  input_name_1, 
  ...,
  input_name_n,
  output_name_1,
  ...,
  output_name_m
);

input  input_name_1;        // begin input ports definition
...
input  input_name_n;        // end input ports definition

output output_name_1;       // begin output ports definition
...
output output_name_m;       // end output ports definition

wire wire_name_1;           // begin wire (net) definition
...
wire wire_name_k;           // end wire (net) definition

cell_type cell_instance_name ( .pin_name (net name), ... );
...

end module                  // end module definition
```

The admissible structure of the verilog file is to start with a module declaration, 
which defines the module interface with name `<circuit_name>`.
The input and output pins are explicitly declared with the keywords `input` and `output`.
The internal nets are declared with the keyword `wire`.
For each instance, `cell_type` and every cell pin `pin_name` should be found in the library.
Connection between instances are described in terms of wire-pin mapping.

## Example

The following example demonstrates a valid verilog format for OpenTimer.

```verilog
module simple (inp1, inp2, tau2015_clk, out);

// Start PIs
input inp1;           // declare the inp1 direction
input inp2;           // declare the inp2 direction
input tau2015_clk;    // declare the clock direction

// Start POs
output out;           // deckare the out direction

// Start wires
wire n1;
wire n2;
wire n3;
wire n4;
wire inp1;
wire inp2;
wire tau2015_clk;
wire out;

// Start cells
NAND2_X1 u1 ( .a(inp1), .b(inp2), .o(n1) );
DFF_X80 f1 ( .d(n2), .ck(tau2015_clk), .q(n3) );
INV_X1 u2 ( .a(n3), .o(n4) );
INV_X2 u3 ( .a(n4), .o(out) );
NOR2_X1 u4 ( .a(n1), .b(n3), .o(n2) );

endmodule
```

<p>
<img src="verilog_example.png" width="70%">
Diagram of the design.
</p>

The example declares a module *simple* with four ports:
+ primary input port `inp1`
+ primary input port `inp2`
+ primary input port `tau2015_clk`
+ primary output port `out`

The design contains eight nets for internal connections:
+ wire `n1` connecting instance `u1` and instance `u4`
+ wire `n2` connecting instance `f1` and instance `u4`
+ wire `n3` connecting instance `f1`, instance `u2`, and instance `u4`
+ wire `n4` connecting instance `u2` and instance `u3` 
+ wire `inp1` connecting primary input port `inp1` and instance `u1`
+ wire `inp2` connecting primary input port `inp2` and instance `u1`
+ wire `out` connecting primary output port `out` and instance `u3`

The design uses five cells:
+ instance `u1` of cell `NAND2_X1`, connecting to instance `u4` and ports `inp1` and `inp2`
+ instance `f1` of cell `DFF_X80`, connecting to port `tau2015_clk`, instances `u4` and `u2`
+ instance `u2` of cell `INV_X1`, connecting to instances `f1` and `u3`
+ instance `u3` of cell `INV_X2`, connecting to instance `u2` and port `out`
+ instance `u4` of cell `NOR2_X1`, connecting to instances `u1` and `f1`

# Work In Progress

We are currently improving OpenTimer's capability of parsing verilog files.
This includes support for:

+ Array syntax and bus declaration.
+ Hierarchical designs with multiple modules.



# Reference
1. [2015 ACM TAU Timing Analysis Contest][TAU15]

* * *

[TAU15]:        https://sites.google.com/site/taucontest2015/






