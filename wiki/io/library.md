# Cell Library (Liberty)

OpenTimer reads a set of Cell Library files (.lib)
that contain all cells available to the design.
Each instance declared in the verilog netlist (.v) must have 
a corresponding cell type found in the cell library.

+ [Admissible Library Format](#admissible-library-format)
+ [Non-linear Delay Model](#non-linear-delay-model)

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

The example below demonstrates a valid library file (source from [TAU15 Contest][TAU15]) 
for OpenTimer.

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

# Non-linear Delay Model

OpenTimer supports *Non-Linear Delay Model (NLDM)*.
Most of the cell libraries include table models to specify the delays and timing checks
for various timing arcs of the cell.
The table models are referred to as NLDM,
and are used for delay, output slew, and timing tests.
NLDM captures the delay through the combination of input transition time at the cell input pin
and the total output capacitance at the cell output pin.

## How does NLDM Work in OpenTimer?

To give you a better idea about how OpenTimer works with NLDM,
let's start with an example of such a table for a typical inverter cell.

```text
cell ("INV_X1") {
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
For values outside the indices, we perform interpolation or extrapolation to obtain the resulting timing values.

## Table Lookup through Linear Interpolation and Extrapolation

The figure below demonstrates different timing lookup tables of scalar,
one dimension, and two dimensions.

<img src="nldm.png" width="70%"> 

If the table is of size 1x1 (single scalar value),
no interpolation is needed. 
Regardless of input `x` and `y`, the output value `z` is constant.

If the table is one-dimensional (1xn or mx1),
the output values depends on the non-scalar dimension.
For instance, in the above 1x4 table, if `y < y1`, 
the output value z is the linear extrapolation between `z1` and `z2`.
If `y2 ≤ y ≤ y3`, the output value `z` is the linear interpolation 
between `z2` and `z3`.
If `y > y4`, the output value `z` is the linear extrapolation 
between `z3` and `z4`.

If the table is two-dimensional, 
we perform linear interpolation or extrapolation on the x value first,
and then perform the linear interpolation or extrapolation on the y value.
For instance, in the above 3x4 table,
if `x2 < x < x3` and `y2 < y < y3`, 
we compute `z_first` by linear interpolation on `z22` and `z32`,
and `z_second` by linear interpolation on `z23` and `z33`.
Then we determine the output value z by linear interpolation on
`z_first` and `z_second`.

# Reference

1. [2015 ACM TAU Timing Analysis Contest][TAU15]
2. [Synopsys Technology Access Program (TAP-in)][Synopsys TAP-in]

* * *

[Synopsys TAP-in]: https://www.synopsys.com/community/interoperability-programs/tap-in.html
[SDC-Basics]:      http://www.vlsi-expert.com/2011/02/synopsys-design-constraints-sdc-basics.html
[TAU15]:           https://sites.google.com/site/taucontest2015/

