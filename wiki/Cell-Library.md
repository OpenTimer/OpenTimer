# Cell Library

OpenTimer reads a set of Cell Library files (.lib)
that contain all cells available to the design.
Each instance declared in the verilog netlist (.v) must have 
a corresponding cell type found in the cell library.

# Admissible Library Format

OpenTimer has a built-in parser to read in a library file.
A valid library format contains 
1. header section
2. lookup table template definitions
3. cell definitions

A header section has the following syntax.

```text
library (<library name>) {
  
  /* header section */
  delay_model  : table_lookup ;
  time unit    : <time unit>
  voltage_unit : <voltage unit>
  current_unit : <current_unit>
  capacitive_load_unit (<float>, <capacitance unit>)
  leakage_power_unit     : <power unit>  
  pulling_resistive_unit : <resistance unit>
  ...
  nom_process     : <double> ;
  nom_temperature : <double> ;
  nom_voltage     : <double> ;
  operating_conditions(<instance name>) {
    process     : <double> ;
    temperature : <double> ;
    voltage     : <double> ;
  }
  default_operating_conditions : <instance name>

  /* lookup table template definitions */
  lu_table_template (<table label>) {
    variable_1 : <variable name> ;
    index_1 (<string of data points for variable_1>);
    variable_2 : <variable name> ;
    index_2 (<string of data points for variable_2>);
    ...
  }
  ... more template definitions

  /* cell definitions */
  cell (<cell type>) {
    pin (<pin name>) {
      direction       : <direction> ;
      clock           : <boolean>   ;
      max_capacitance : <double>    ;
      min_capacitance : <double>    ;
      ...
      timing() {
        related_pin   : <pin name> ;
        /* combinational or sequential definitions */
      }
      ... more timing definitions
    }
    ... more pin definitions
  }
  ... more cell definitions
}
```

# Timing Model

Liberty has different flavors of models for timing, power, and noise
to abstract the driver and receiver model at a higher level than SPICE.
Standard models include:

+ Non-linear Delay Model (NLDM)
+ Non-linear Power Model (NLPM)
+ Composite Current Source (CCS)
+ Composite Current Power (CCSP)
+ Composite Current Noise (CCSN)

The timing models are specified for each arc of the cell.
Currently, OpenTimer supports only NLDM in computing slew and delay values.

## Non-linear Delay Model (NLDM)

Most of the cell libraries include table model to specify the delays and timing checks
for various timing arcs of the cell.
The table models are referred to as Non-linear Delay Model (NLDM),
and are used for delay, output slew, and timing tests.
NLDM captures the delay through the combination of input transition time at the cell input pin
and the total output capacitance at the cell output pin.
Here is an example of such a table for a typical inverter cell:

```text
pin ("o") {
  direction : output ;
  capacitance : 0.0 ;
  max_capacitance : 12.80 ;
  min_capacitance : 0.00 ;
  timing() {
    cell_fall ("delay_template_5x5") {
      index_1 ("0.00,1.00,2.00,4.00,8.00") ;      // input transition
      index_2 ("5.00,30.00,50.00,80.00,140.00") ; // output capacitance
      values (\
        "9.376,  14.576, 18.136, 22.088, 27.856", \
        "13.544, 18.744, 22.88,  27.96,  35.32",  \
        "17.704, 22.904, 27.064, 32.992, 41.784", \
        "26.04,  31.24,  35.4,   41.64,  52.84",  \
        "42.704, 47.904, 52.064, 58.304, 70.784"  \
      );
    }
    cell_rise ("delay_template_5x5") {
      index_1 ("0.00,1.00,2.00,4.00,8.00") ;      // input transition
      index_2 ("5.00,30.00,50.00,80.00,140.00") ; // output capacitance
      values (\
        "9.376,  14.576, 18.136, 22.088, 27.856", \
        "13.544, 18.744, 22.88,  27.96,  35.32",  \
        "17.704, 22.904, 27.064, 32.992, 41.784", \
        "26.04,  31.24,  35.4,   41.64,  52.84",  \
        "42.704, 47.904, 52.064, 58.304, 70.784"  \
      );
    }
    timing_sense : negative_unate ;
    related_pin  : "a" ;
  }
}
```

The above example describes the delay of from the input pin `a` to the output pin `o`.
Two tables are defined for fall delay and rise delay at pin `o`,
labeled as `cell_fall` and `cell_rise`.
The type of indices and the order of lookup table indices are defined in the 
lookup table template `delay_template_5x5`.

```text
lu_table_template(delay_template_5x5) {
  variable_1 : input_net_transition;
  variable_2 : total_output_net_capacitance;
  index_1 ("1000, 1001, 1002");
  index_2 ("1000, 1001, 1002");
}
```

This lookup table template defines that the first variable is the input transition time
and the second variable is the output capacitance.
Based on the upon delay tables, an input fall transition time of 1.00 (library time unit)
and an output load of 30.00 (library capacitance unit) will correspond to the rise delay
of the inverter of 18.744 (library time unit).
For values outside the indeices, we perform interpolation to obtain the resulting timing values.

# Reference

1. [2015 ACM TAU Timing Analysis Contest][TAU15]
2. [Synopsys Technology Access Program (TAP-in)][Synopsys TAP-in]
3. [Liberty User Guides Version 2017.06][Liberty User Guide]

* * *

[Synopsys TAP-in]: https://www.synopsys.com/community/interoperability-programs/tap-in.html
[SDC-Basics]:      http://www.vlsi-expert.com/2011/02/synopsys-design-constraints-sdc-basics.html
[TAU15]:           https://sites.google.com/site/taucontest2015/
[Liberty User Guide]: reference/Liberty_User_Guide_2017_06.pdf

