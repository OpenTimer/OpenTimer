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

## Example

The example below demonstrates a valid library file for OpenTimer.

```text
library ("simple") {
  delay_model : table_lookup ; 
  time_unit : "1ps" ; 
  voltage_unit : "1V" ; 
  current_unit : "1mA" ; 
  leakage_power_unit : 1uW ; 
  capacitive_load_unit(1,ff);
  pulling_resistance_unit : "1kohm" ; 
  default_fanout_load : 1.0 ; 
  default_inout_pin_cap : 0.0 ; 
  default_input_pin_cap : 0.0 ; 
  default_output_pin_cap : 0.0 ; 
  slew_lower_threshold_pct_rise : 20.0 ; 
  slew_lower_threshold_pct_fall : 20.0 ; 
  slew_upper_threshold_pct_rise : 80.0 ; 
  slew_upper_threshold_pct_fall : 80.0 ; 
  input_threshold_pct_rise : 50.0 ; 
  input_threshold_pct_fall : 50.0 ; 
  output_threshold_pct_rise : 50.0 ; 
  output_threshold_pct_fall : 50.0 ; 
  nom_voltage : 0.7 ; 
  nom_temperature : 70.0 ; 
  nom_process :  1.0 ;

  operating_conditions("typical_1.00") { 
    process : 1.00 ; 
    temperature : 70.0 ; 
    voltage : 0.7 ; 
    tree_type : "balanced_tree" ; 
  }

  default_operating_conditions : "typical_1.00" ; 

  lu_table_template (delay_outputslew_template_7X8) {
    variable_1 : total_output_net_capacitance ;
    variable_2 : input_net_transition ;
    index_1 ("1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6");
    index_2 ("2.0, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7");
  }

  /* Begin cell: INV_X1 */
  cell ("INV_X1") {
    pin ("o") {
      direction : output ;
      capacitance : 0.0 ;
      max_capacitance : 12.80 ;
      min_capacitance : 0.00 ;
      timing() {
        cell_fall ("delay_outputslew_template_7X8") {
          index_1 ("0.00,1.00,2.00,4.00,8.00,16.00,32.00") ;
          index_2 ("5.00,30.00,50.00,80.00,140.00,200.00,300.00,500.00") ;
          values (\
            "9.376, 14.576, 18.136, 22.088, 27.856, 32.352, 38.568, 48.992",\
            "13.544, 18.744, 22.88, 27.96, 35.32, 40.944, 48.52, 60.664",\
            "17.704, 22.904, 27.064, 32.992, 41.784, 48.456, 57.336, 71.2",\
            "26.04, 31.24, 35.4, 41.64, 52.84, 61.408, 72.68, 89.872",\
            "42.704, 47.904, 52.064, 58.304, 70.784, 82.472, 97.92, 121.136",\
            "76.04, 81.24, 85.4, 91.64, 104.12, 116.6, 137.272, 170.648",\
            "142.704, 147.904, 152.064, 158.304, 170.784, 183.264, 204.064, 245.664"\
          );
        }
        fall_transition ("delay_outputslew_template_7X8") {
          index_1 ("0.00,1.00,2.00,4.00,8.00,16.00,32.00") ;
          index_2 ("5.00,30.00,50.00,80.00,140.00,200.00,300.00,500.00") ;
          values (\
            "10, 10.976, 13.104, 16.08, 20.136, 22.92, 26.36, 31.864",\
            "15, 15.36, 16.92, 20.224, 25.72, 29.648, 34.384, 41.048",\
            "20, 20.072, 21.128, 23.928, 30.376, 35.272, 41.328, 49.488",\
            "30, 30, 30.256, 32.08, 38.128, 44.616, 52.912, 64.456",\
            "50, 50, 50, 50.32, 54.008, 59.808, 71.024, 88.184",\
            "90, 90, 90, 90, 90.448, 93.336, 101.536, 123.584",\
            "170, 170, 170, 170, 170, 170, 172.12, 185.672"\
          );
        }
        cell_rise ("delay_outputslew_template_7X8") {
          index_1 ("0.00,1.00,2.00,4.00,8.00,16.00,32.00") ;
          index_2 ("5.00,30.00,50.00,80.00,140.00,200.00,300.00,500.00") ;
          values (\
            "9.376, 14.576, 18.136, 22.088, 27.856, 32.352, 38.568, 48.992",\
            "13.544, 18.744, 22.88, 27.96, 35.32, 40.944, 48.52, 60.664",\
            "17.704, 22.904, 27.064, 32.992, 41.784, 48.456, 57.336, 71.2",\
            "26.04, 31.24, 35.4, 41.64, 52.84, 61.408, 72.68, 89.872",\
            "42.704, 47.904, 52.064, 58.304, 70.784, 82.472, 97.92, 121.136",\
            "76.04, 81.24, 85.4, 91.64, 104.12, 116.6, 137.272, 170.648",\
            "142.704, 147.904, 152.064, 158.304, 170.784, 183.264, 204.064, 245.664"\
          );
        }
        rise_transition ("delay_outputslew_template_7X8") {
          index_1 ("0.00,1.00,2.00,4.00,8.00,16.00,32.00") ;
          index_2 ("5.00,30.00,50.00,80.00,140.00,200.00,300.00,500.00") ;
          values (\
            "10, 10.976, 13.104, 16.08, 20.136, 22.92, 26.36, 31.864",\
            "15, 15.36, 16.92, 20.224, 25.72, 29.648, 34.384, 41.048",\
            "20, 20.072, 21.128, 23.928, 30.376, 35.272, 41.328, 49.488",\
            "30, 30, 30.256, 32.08, 38.128, 44.616, 52.912, 64.456",\
            "50, 50, 50, 50.32, 54.008, 59.808, 71.024, 88.184",\
            "90, 90, 90, 90, 90.448, 93.336, 101.536, 123.584",\
            "170, 170, 170, 170, 170, 170, 172.12, 185.672"\
          );
        }
        timing_sense : negative_unate ;
        related_pin  : "a" ;
      }
      /* End timing */
    }
    /* End pin */
    pin ("a") {
      capacitance : 1.00 ;
      direction : input ;
    }
    /* End pin */
  }
  /* End cell: INV_X1 */
}
```

In the header section, the library `simple` defines the following units:
+ 1 pico second (ps) time unit
+ 1 volt (V) voltage unit
+ 1 milli ampere (mA) current unit
+ 1 micro watt (uW) power unit
+ 1 femto farad (ff) capacitance unit
+ 1 kilo ohm (kohm) resistance unit

In the lookup table template section, the library defines one template
`delay_outputslew_template_7x8`.
The first variable `total_output_net_capacitance` refers to the output capacitance and
the second variable `input_net_transition` refers to the input slew.
The table has seven data indices for the output capacitance and 
eight data indices for the input slew.

In the cell section, the library defines one combinational cell `INV_X1` (inverter of size one).
This cell has the following pins:
+ pin `a` with output direction, 1.00 ff capacitance
+ pin `o` with input  direction, 0.00 ff capacitance

Pin `a` has no internal timing definitions (cell arcs) and pin `o` has one timing arc
coming from `a` (related pin) with the following attributes:
+ `cell_fall` defines the delay value at the fall transition of pin `o`
+ `cell_rise` defines the delay value at the rise transition of pin `o`
+ `fall_transition` defines the slew value at the fall transition of pin `o`
+ `rise_transition` defines the slew value at the rise transition of pin `o`

All cells including both combinational and sequential elements follow the similar definitions.

# Timing Model

Liberty has different flavors of models for timing, power, and noise
to abstract the driver and receiver model at a higher level than SPICE.
Standard models include:
+ Non-linear Delay Model (NLDM)
+ Non-linear Power Model (NLPM)
+ Composite Current Source (CCS)
+ Composite Current Power (CCSP)
+ Composite Current Noise (CCSN)

Currently, OpenTimer supports only NLDM in computing slew and delay values.

# Reference

1. [2015 ACM TAU Timing Analysis Contest][TAU15]
2. [Synopsys Technology Access Program (TAP-in)][Synopsys TAP-in]
3. [Liberty User Guides Version 2017.06][Liberty User Guide]

* * *

[Synopsys TAP-in]: https://www.synopsys.com/community/interoperability-programs/tap-in.html
[SDC-Basics]:      http://www.vlsi-expert.com/2011/02/synopsys-design-constraints-sdc-basics.html
[TAU15]:           https://sites.google.com/site/taucontest2015/
[Liberty User Guide]: reference/Liberty_User_Guide_2017_06.pdf
