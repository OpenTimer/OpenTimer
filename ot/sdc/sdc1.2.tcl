################################################################################
#                       >  COPYRIGHT NOTICE  <
#
#  Copyright 2000 (c) Synopsys.
#
#
################################################################################
#
# Title        : SDC-parser commands declaration for SDC version 1.2
#
# Project      : SDC-parser
#
# Authors      : A.Gratchev
#                A.Sokhatski
#                Intrinsix Corp.
#                anso@intrinsix.com  
#
################################################################################


 declare all_clocks {
 }

 declare all_inputs {
  {-level_sensitive     Flag       }
  {-edge_triggered      Flag       }
  {-clock               List       }
 } {!(param(-level_sensitive) && param(-edge_triggered))}

 declare all_outputs {
  {-level_sensitive     Flag       }
  {-edge_triggered      Flag       }
  {-clock               List       }
 } {!(param(-level_sensitive) && param(-edge_triggered))}

 declare create_clock {
  {-period              Float      {$par>=0}}
  {-name                String     }
  {-waveform            List       {type_Float {length($length>=2 && ($length % 2)==0)} } }
  {port_pin_list        List       }
 } {param(-period) && (param(-name) || param(port_pin_list))}

 declare current_design {
 }

 declare current_instance {
  {-instance            String     }
 }

 declare get_cells {
  {-of_objects          List       }
  {patterns             List       }
  {-hierarchical        Flag       }
  {-hsc                 Enum       {{/} {@} {^} {#} {.} {|}}}
 } {(param(patterns) && !param(-of_objects)) || \
    (param(-of_objects) && !param(patterns))}
# && !param(-hierarchical))}

 declare get_clocks {
  {patterns             List       }
 } {param(patterns)}

 declare get_lib_cells {
  {patterns             List       }
  {-hsc                 Enum       {{/} {@} {^} {#} {.} {|}}}
 } {param(patterns)}

 declare get_lib_pins {
  {patterns             List       }
  {-hsc                 Enum       {{/} {@} {^} {#} {.} {|}}}
 } {param(patterns)}

 declare get_libs {
  {patterns             List       }
 } {param(patterns)}

 declare get_nets {
  {patterns             List       }
  {-hierarchical        Flag       }
  {-hsc                 Enum       {{/} {@} {^} {#} {.} {|}}}
 } {param(patterns)}

 declare get_pins {
  {patterns             List       }
  {-hierarchical        Flag       }
  {-hsc                 Enum       {{/} {@} {^} {#} {.} {|}}}
 } {param(patterns)}

 declare get_ports {
  {patterns             List       }
 } {param(patterns)}

 declare set_case_analysis {
  {value                Enum        {0 1 rising falling zero one rise fall}}
  {port_pin_list        List        }
 } {param(value) && param(port_pin_list)}

 declare set_clock_gating_check {
  {-setup               Float       {$par>=0}}
  {-hold                Float       {$par>=0}}
  {-rise                Flag        }
  {-fall                Flag        }
  {-high                Flag        }
  {-low                 Flag        }
  {object_list          List        }
 } {(param(-setup) || param(-hold) || param(-high) || param(-low)) && \
   !(param(-high) && param(-low))}

 declare set_clock_latency {
  {delay                Float       {1}}
  {object_list          List        }
  {-rise                Flag        }
  {-fall                Flag        }
  {-min                 Flag        }
  {-max                 Flag        }
  {-source              Flag        }
  {-early               Flag        }
  {-late                Flag        }
 } {param(delay) && param(object_list)}

 declare set_clock_transition {
  {transition           Float       {1}}
  {clock_list           List        }
  {-rise                Flag        }
  {-fall                Flag        }
  {-min                 Flag        }
  {-max                 Flag        }
 } {param(transition) && param(clock_list) && \
   !(param(-rise) && param(-fall))}

 declare set_clock_uncertainty {
  {uncertainty          Float       {1}}
  {-from                List        }
  {-to                  List        }
  {-rise                Flag        }
  {-fall                Flag        }
  {-setup               Flag        }
  {-hold                Flag        }
  {object_list          List        }
 } {param(uncertainty) && !(param(object_list) && (param(-from) || param(-to)))}

 declare set_disable_timing {
  {object_list          List        }
  {-from                String      }
  {-to                  String      }
 } {param(object_list) && \
   !(param(-to) ^ param(-from))}

 declare set_drive {
  {resistance           Float       {$par>=0}}
  {port_list            List        }
  {-rise                Flag        }
  {-fall                Flag        }
  {-min                 Flag        }
  {-max                 Flag        }
 } {param(resistance) && param(port_list)}

 declare set_driving_cell {
  {-lib_cell            String      }
  {-rise                Flag        }
  {-fall                Flag        }
  {-library             String      }
  {-pin                 String      }
  {-from_pin            String      }
  {-multiply_by         Float       {$par>=0}}
  {-dont_scale          Flag        }
  {-no_design_rule      Flag        }
  {-input_transition_rise   Float   {$par>=0}}
  {-input_transition_fall   Float   {$par>=0}}
  {port_list            List        }
 } {param(port_list)}

 declare set_false_path {
  {-setup               Flag        }
  {-hold                Flag        }
  {-rise                Flag        }
  {-fall                Flag        }
  {-from                List        }
  {-to                  List        }
  {-through             List        {dup}}
 } {(param(-from) || param(-to) || param(-through)) && \
 !(param(-rise) && param(-fall)) && !(param(-setup) && param(-hold))}

 declare set_fanout_load {
  {value                Float       {$par>=0}}
  {port_list            List        }
 } {param(value) && param(port_list)}

 declare set_hierarchy_separator {
  {hchar                Enum        {{/} {@} {^} {#} {.} {|}}}
 } {param(hchar)}

 declare set_input_delay {
  {-clock               List        }
  {-clock_fall          Flag        }
  {-level_sensitive     Flag        }
  {-rise                Flag        }
  {-fall                Flag        }
  {-min                 Flag        }
  {-max                 Flag        }
  {-add_delay           Flag        }
  {delay_value          Float       {1}}
  {port_pin_list        List        }
 } {param(delay_value) && param(port_pin_list) && \
 !((param(-clock_fall) || param(-level_sensitive)) && !param(-clock))}

 declare set_input_transition {
  {-rise                Flag        }
  {-fall                Flag        }
  {-min                 Flag        }
  {-max                 Flag        }
  {transition           Float       {$par>=0}}
  {port_list            List        }
 } {param(transition) && param(port_list)}

 declare set_load {
  {-min                 Flag        }
  {-max                 Flag        }
  {-substract_pin_load  Flag        }
  {-pin_load            Flag        }
  {-wire_load           Flag        }
  {value                Float       {$par>=0}}
  {objects              List        }
 } {param(value) && param(objects)}

 declare set_logic_dc {
  {port_list            List        }
 } {param(port_list)}

 declare set_logic_one {
  {port_list            List        }
 } {param(port_list)}

 declare set_logic_zero {
  {port_list            List        }
 } {param(port_list)}

 declare set_max_area {
  {area_value           Float       {$par>=0}}
 } {param(area_value)}

 declare set_max_capacitance {
  {capacitance_value    Float       {$par>=0}}
  {object_list          List        }
 } {param(capacitance_value) && param(object_list)}

 declare set_max_delay {
  {delay_value          Float       {1}}
  {-rise                Flag        }
  {-fall                Flag        }
  {-from                List        }
  {-to                  List        }
  {-through             List        {dup}}
 } {param(delay_value) && \
 !(param(-rise) && param(-fall))}

 declare set_max_fanout {
  {fanout_value         Float       {$par>=0}}
  {object_list          List        }
 } {param(fanout_value) && param(object_list)}

 declare set_max_time_borrow {
  {delay_value          Float       {$par>=0}}
  {object_list          List        }
 } {param(delay_value) && param(object_list)}

 declare set_max_transition {
  {transition_value     Float       {$par>=0}}
  {object_list          List        }
 } {param(transition_value) && param(object_list)}

 declare set_min_capacitance {
  {capacitance_value    Float       {$par>=0}}
  {object_list          List        }
 } {param(capacitance_value) && param(object_list)}

 declare set_min_delay {
  {delay_value          Float       {1}}
  {-rise                Flag        }
  {-fall                Flag        }
  {-from                List        }
  {-to                  List        }
  {-through             List        {dup}}
 } {param(delay_value) && \
  !(param(-rise) && param(-fall))}

 declare set_multicycle_path {
  {path_multiplier      Int         {1}}
  {-setup               Flag        }
  {-hold                Flag        }
  {-rise                Flag        }
  {-fall                Flag        }
  {-start               Flag        }
  {-end                 Flag        }
  {-from                List        }
  {-to                  List        }
  {-through             List        {dup}}
 } {param(path_multiplier)}

 declare set_operating_conditions {
  {-analysis_type       Enum        {single bc_wc on_chip_variation}}
  {-library             List        }
  {-max                 String      }
  {-min                 String      }
  {-max_library         List        }
  {-min_library         List        }
  {condition            String      }
 }

 declare set_output_delay {
  {-clock               List        }
  {-clock_fall          Flag        }
  {-level_sensitive     Flag        }
  {-rise                Flag        }
  {-fall                Flag        }
  {-min                 Flag        }
  {-max                 Flag        }
  {-add_delay           Flag        }
  {delay_value          Float       {1}}
  {port_pin_list        List        }
 } {param(delay_value) && param(port_pin_list) && \
 !((param(-clock_fall) || param(-level_sensitive)) && !param(-clock))}

 declare set_port_fanout_number {
  {fanout_number        Int         {($par>=0) && ($par<=100000)}}
  {port_list            List        }
 } {param(fanout_number) && param(port_list)}

 declare set_propagated_clock {
  {object_list          List        }
 } {param(object_list)}

 declare set_resistance {
  {value                Float       {$par>=0}}
  {net_list             List        }
  {-min                 Flag        }
  {-max                 Flag        }
 } {param(value) && param(net_list)}

 declare set_wire_load_min_block_size {
  {size                 Float       {$par>=0}}
 } {param(size)}

 declare set_wire_load_mode {
  {mode_name            Enum        {top enclosed segmented}}
 } {param(mode_name)}

 declare set_wire_load_model {
  {-name                String      }
  {-library             List        }
  {-min                 Flag        }
  {-max                 Flag        }
  {object_list          List        }
 } {param(-name)}

 declare set_wire_load_selection_group {
  {-group_name          String      }
  {-library             List        }
  {-min                 Flag        }
  {-max                 Flag        }
  {object_list          List        }
 } {param(-name)}

# register_new_type EdgeList {
#    if {[sdc::check_type List $value]} {
#        if {[expr {[llength $value] % 2}] || [llength $value]==0} {
#            return 0
#        }
#
#        set startvalue -0.1
#
#        foreach parv $value {
#            if {[catch {expr {$parv + 1}}]} {
#                return 0
#            }
#            if {$parv<=$startvalue} {
#                return 0
#            }
#            set startvalue $parv
#        }
#
#        return 1
#    } else {
#        return 0
#    }
# }
