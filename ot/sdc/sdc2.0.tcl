################################################################################
#
#  Copyright 2013(c) Synopsys, Inc.
#
################################################################################
#
# Title        : SDC-parser commands declaration for SDC version 2.0
#
# Update By    : Ibna Faruque
# Updated On   : July, 2013
# Bug Fixes    : -combinational for create_generated_clock
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

#added on SDC1.7
declare all_registers {
  {-clock               List      }
  {-rise_clock		List	  }
  {-fall_clock          List      }
  {-no_hierarchy        Flag      }
  {-cells	 	Flag      }
  {-data_pins           Flag      }
  {-clock_pins          Flag      }
  {-slave_clock_pins    Flag      }
  {-async_pins          Flag      }
  {-output_pins         Flag      }
  {-level_sensitive     Flag      }
  {-edge_triggered      Flag      }
  {-master_slave        Flag      }
 }
#added -comment for sdc1.9
 declare create_clock {
  {-period              Float      {$par>=0}	}
  {-name                String     		}
  {-comment		String                  }
  {-waveform            List       {type_Float {length($length>=2 && ($length % 2)==0)} } }
  {port_pin_list        List       		}
  {-add                 Flag       		}
 } {param(-period) && (param(-name) || param(port_pin_list))}

 declare create_generated_clock {
 {-name                String     		}
 {-source	       List       		}
 {-edges	       List  {type_Float {length($length>=2} }	  }
 {-divide_by	       Int   {1 && $par>=0}  	}
 {-multiply_by	       Int   {1 && $par>=0} 	}
 {-edge_shift          List       		}
 {-duty_cycle          Float {$par>=0 && $par <=100} }
 {-invert              Flag      		}
 {-combinational       Flag                     }
 {port_pin_list        List       		}
 {-add                 Flag       		}
 {-comment             String                   }
 {-master_clock        List       		} 
} {param(-source) && param(port_pin_list) && !(param(-multiply_by) && param(-divide_by))}

 declare current_design {
 }

 declare current_instance {
  {-instance           String     		}
 }

# added SDC1.7
  declare group_path {
  {-name	       String     		}
  {-default	       Flag      		}
  {-weight             Float {$par>=0.0 && $par <=100.0}    } 
  {-from               List    			} 
  {-rise_from          List     		} 
  {-fall_from          List     		}	 
  {-to                 List     		} 
  {-rise_to            List     		} 
  {-fall_to            List     		} 
  {-through            List    	{dup}     	} 
  {-rise_through       List    	{dup}		} 
  {-fall_through       List    	{dup}		} 
  {-comment            String                   }
 } {((param(-name) && !param(-default)) || \
    (param(-default) && !param(-name)) || !param(-name) && \
    (param(-from) ^ param(-rise_from) ^ param(-fall_from)))}


 declare get_cells {
  {-of_objects          List       }
  {patterns             List       }
  {-hierarchical        Flag       }
  {-regexp              Flag       }
  {-nocase              Flag       }
  {-hsc                 Enum       {{/} {@} {^} {#} {.} {|}}}
 # {                                }
 } {(param(patterns) && !param(-of_objects)) || \
    (param(-of_objects) && !param(patterns)) || !param(patterns)}
# && !param(-hierarchical))}

# get_cell synonym for get_cells
 declare get_cell {
  {-of_objects          List       }
  {patterns             List       }
  {-hierarchical        Flag       }
  {-regexp              Flag       }
  {-nocase              Flag       }
  {-hsc                 Enum       {{/} {@} {^} {#} {.} {|}}}
  } {(param(patterns) && !param(-of_objects)) || \
    (param(-of_objects) && !param(patterns)) || !param(patterns)}



 declare get_clocks {
  {patterns             List       }
  {-regexp              Flag       }
  {-nocase              Flag       }
 } 

 declare get_clock {
  {patterns             List       }
  {-regexp              Flag       }
  {-nocase              Flag       } 
}

 declare get_lib_cells {
  {patterns             List       }
  {-regexp              Flag       } 
  {-nocase              Flag       }
  {-hsc                 Enum       {{/} {@} {^} {#} {.} {|}}}
 } {param(patterns)}

 declare get_lib_cell {
  {patterns             List       }
  {-regexp              Flag       } 
  {-nocase              Flag       }
  {-hsc                 Enum       {{/} {@} {^} {#} {.} {|}}}
 } {param(patterns)}


 declare get_lib_pins {
  {patterns             List       }
  {-regexp              Flag       } 
  {-nocase              Flag       }
  {-hsc                 Enum       {{/} {@} {^} {#} {.} {|}}}
 } {param(patterns)}

 declare get_lib_pin {
  {patterns             List       }
  {-regexp              Flag       } 
  {-nocase              Flag       }
  {-hsc                 Enum       {{/} {@} {^} {#} {.} {|}}}
 } {param(patterns)}


 declare get_libs {
  {-regexp              Flag       } 
  {-nocase              Flag       }
  {patterns             List       }
}

 declare get_nets {
  {patterns             List       }
  {-of_objects          List       }
  {-hierarchical        Flag       }
  {-regexp              Flag       } 
  {-nocase              Flag       }
  {-hsc                 Enum       {{/} {@} {^} {#} {.} {|}}}
} {(param(patterns) && !param(-of_objects)) || \
    (param(-of_objects) && !param(patterns)) || !param(patterns)}


 declare get_net {
  {patterns             List       }
  {-of_objects          List       }
  {-hierarchical        Flag       }
  {-regexp              Flag       } 
  {-nocase              Flag       }
  {-hsc                 Enum       {{/} {@} {^} {#} {.} {|}}}
} {(param(patterns) && !param(-of_objects)) || \
    (param(-of_objects) && !param(patterns)) || !param(patterns)}



 declare get_pins {
  {patterns             List       }
  {-hierarchical        Flag       }
  {-regexp              Flag       } 
  {-nocase              Flag       }
  {-hsc                 Enum       {{/} {@} {^} {#} {.} {|}}}
 } 

 declare get_pin {
  {patterns             List       }
  {-hierarchical        Flag       }
  {-regexp              Flag       } 
  {-nocase              Flag       }
  {-hsc                 Enum       {{/} {@} {^} {#} {.} {|}}}
 } 

 declare get_ports {
  {-hierarchical        Flag       }
  {-regexp              Flag       }
  {patterns             List       }
}
# } {param(patterns)}

 declare get_port {
  {-hierarchical        Flag       }
  {-regexp              Flag       }
  {patterns             List       }
}
# } {param(patterns)}

# sdc1.7 updte
 declare set_units {
  {-capacitance         String       }
  {-resistance          String       }
  {-time                String       }
  {-voltage             String       }
  {-current             String       }
  {-power               String       }
} 

# set_unit synonym for set_units
 declare set_unit {
  {-capacitance         String       }
  {-resistance          String       }
  {-time                String       }
  {-voltage             String       }
  {-current             String       }
  {-power               String       }
} 

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

# sdc1.7 command
 declare set_clock_groups {
  {-name                String       		}
  {-physically_exclusive                Flag    }
  {-logically_exclusive                 Flag    }
  {-asynchronous                	Flag    }
  {-allow_paths                 	Flag    }
  {-group                      	 	List  {dup}  }
  {-comment                             String  } 
 } {(param(-physically_exclusive) ^ param(-logically_exclusive) ^ param(-asynchronous)) }

# set_clock_group synonym for  set_clock_groups
 declare set_clock_group {
  {-name                	String 		 }
  {-physically_exclusive        Flag      	 }
  {-logically_exclusive         Flag       	 }
  {-asynchronous                Flag       	 }
  {-allow_paths                 Flag       	 }
  {-group                       List       	 }
  {-comment                             String   }
 } {(param(-physically_exclusive) ^ param(-logically_exclusive) ^ param(-asynchronous)) }

# sdc1.7 command
 declare set_clock_sense {
  {-clocks                  List	       }
  {-positive                Flag      }
  {-negative                Flag        }
  {-stop_propagation        Flag        }
  {-pulse                   Enum	{rise_triggered_high_pulse rise_triggered_low_pulse \ 
                                 fall_triggered_high_pulse fall_triggered_low_pulse}        }
  {pins                     List        }
 } {(param(-positive) ^ param(-negative) ^ param(-stop_propagation) ^ param(-pulse)) }

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
  {-clock		List        }
 } {param(delay) && param(object_list)}

#sdc1.7
 declare set_ideal_latency {
  {value                Float       {1}}
  {object_list          List           }
  {-rise                Flag           }
  {-fall                Flag           }
  {-min                 Flag           }
  {-max                 Flag           }
 } {param(value) && param(object_list)}

#sdc1.7
 declare set_ideal_network {
  {object_list          List        }
  {-no_propagate                 Flag        }
 } {param(object_list)}

#sdc1.7
 declare set_ideal_transition {
  {value                Float       {1}}
  {object_list          List        }
  {-rise                Flag        }
  {-fall                Flag        }
  {-min                 Flag        }
  {-max                 Flag        }
 } {param(value) && param(object_list)}

#sdc1.8
 declare set_voltage {
  {value                Float       {1}}
  {-object_list          List        }
  {-min                 Float       {$par>=0}  }
 } {param(value) && param(-object_list)}

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
  {-rise_to             List        }
  {-fall_to             List        }
  {-rise_from           List        }
  {-fall_from           List        }
  {-rise                Flag        }
  {-fall                Flag        }
  {-setup               Flag        }
  {-hold                Flag        }
  {object_list          List        }
 } {param(uncertainty) && param(object_list) ^ \
   (((param(-from) ^ param(-rise_from) ^ param(-fall_from))) \
    && (param(-to) ^ param(-rise_to) ^ param(-fall_to)))}

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
  {-lib_cell            	String      }
  {-rise                	Flag        }
  {-fall                	Flag        }
  {-library             	String      }
  {-pin                 	String      }
  {-from_pin            	String      }
  {-multiply_by         	Float       {$par>=0}}
  {-dont_scale          	Flag        }
  {-no_design_rule      	Flag        }
  {-input_transition_rise   	Float   {$par>=0}}
  {-input_transition_fall   	Float   {$par>=0}}
  {port_list            	List        }
  {-min                 	Flag        }
  {-max                 	Flag        }
  {-clock               	List        }
  {-clock_fall          	Flag        }
 } {param(port_list) &&  param(-lib_cell)} 

 declare set_false_path {
  {-setup               	Flag        }
  {-hold                	Flag        }
  {-rise                	Flag        }
  {-fall                	Flag        }
  {-from                	List        }
  {-rise_from               	List        }
  {-fall_from               	List        }
  {-to                  	List        }
  {-rise_to                 	List        }
  {-fall_to                 	List        }
  {-through             	List {dup}  }
  {-fall_throough               List {dup}  }
  {-rise_throough               List {dup}  }
  {-comment                     String      }
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
  {-clock               	List        }
  {-clock_fall          	Flag        }
  {-level_sensitive     	Flag        }
  {-rise                	Flag        }
  {-fall                	Flag        }
  {-min                 	Flag        }
  {-max                 	Flag        }
  {-add_delay           	Flag        }
  {delay_value          	Float   {1} }
  {-reference_pin               List        }
  {port_pin_list        	List        }
  {-network_latency_included    Flag  	    }
  {-source_latency_included     Flag        }
 } {param(delay_value) && param(port_pin_list) && \
 !((param(-clock_fall) || param(-level_sensitive)) && !param(-clock))}

 declare set_input_transition {
  {-rise                Flag        }
  {-fall                Flag        }
  {-min                 Flag        }
  {-max                 Flag        }
  {transition           Float       {$par>=0}}
  {port_list            List        }
  {-clock               List        }
  {-clock_fall          Flag        }
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
  {delay_value          Float       {1}	}
  {-rise                Flag        	}
  {-fall                Flag        	}
  {-from                List        	}
  {-rise_from           List     	}
  {-fall_from           List     	}
  {-to                  List        	}
  {-rise_to             List     	}
  {-fall_to             List     	}
  {-through             List     {dup}	}
  {-fall_through        List 	 {dup}  }
  {-rise_through        List 	 {dup}  }
  {-comment             String         }
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
  {-clock_path          Flag  }
  {-fall                Flag }
  {-rise                Flag}
  {object_list          List        }
 } {param(transition_value) && param(object_list)}

 declare set_min_capacitance {
  {capacitance_value    Float       {$par>=0}}
  {object_list          List        }
 } {param(capacitance_value) && param(object_list)}

 declare set_min_delay {
  {delay_value          Float       {1}	}
  {-rise                Flag       	}
  {-fall                Flag        	}
  {-from                List        	}
  {-rise_from           List     	}
  {-fall_from           List     	}
  {-to                  List        	}
  {-rise_to             List     	}
  {-fall_to             List     	}
  {-through             List      {dup}	}
  {-fall_through        List 	  {dup} }
  {-rise_through        List      {dup} }
  {-comment             String          }
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
  {-rise_to             List        }
  {-fall_to             List        }
  {-rise_from           List        }
  {-fall_from           List        }
  {-rise_through        List   {dup}}
  {-fall_through        List   {dup}}
  {-through             List   {dup}}
  {-comment             String      }
 } {param(path_multiplier)}

 declare set_operating_conditions {
  {-analysis_type       Enum        {single bc_wc on_chip_variation}}
  {-library             List        }
  {-max                 String      }
  {-min                 String      }
  {-max_library         List        }
  {-object_list		List        }
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
  {-reference_pin       List        }
  {port_pin_list        List        }
  {-network_latency_included     Flag  }
  {-source_latency_included      Flag  }

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
  {group_name          String       }
  {-library             List        }
  {-min                 Flag        }
  {-max                 Flag        }
  {object_list          List        }
 } {param(group_name)}

 declare set_data_check {
  {-from                List        }
  {-rise_from           List        }
  {-fall_from           List        }
  {-to                  List        }
  {-rise_to             List        }
  {-fall_to             List        }
  {-rise                Flag        }
  {-fall                Flag        }
  {-setup               Flag        }
  {-hold                Flag        }
  {-clock               List        }
  {value                Float  {$par>=0}      }
 } {(param(-rise_from) || param(-from) || param(-fall_from)) && \
   ((param(-to) || param(-rise_to)) || param(-fall_to)) }

 declare set_timing_derate {
  {derate_value        Float  {$par>=0.1  } }
  {object_list          List      }
# {derate_value         Float {$par>=0.1 && $par<=2.0}}
  {-min                 Flag      }
  {-max                 Flag      }
  {-rise                Flag      }
  {-fall                Flag      }
  {-early               Flag      }
  {-late                Flag      }
  {-clock               Flag      }
  {-data                Flag      }
  {-net_delay           Flag      }
  {-cell_delay          Flag      }
  {-cell_check          Flag      }
} {param(derate_value) && (param(-early) && !param(-late)) || (param(-late) && !param(-early))}



 declare set_max_dynamic_power {
  {power_value           Float {$par>=0}       }
  {-unit                 Enum  {GW MW KW W mW uW nW pW fW aW} }
 } {param(power_value)}

 declare set_max_leakage_power {
  {power_value         Float        {$par>=0}}
  {-unit               Enum         {GW MW KW W mW uW nW pW fW aW}}
 } {param(power_value)}

declare create_voltage_area {
  {-name                String {1}    	}
  {-coordinate          List       	}
  {-guard_band_x        List       	}
  {-guard_band_y        List       	}
  {cell_list            List {1}      	}
 } {param(-name) && {param(cell_list)}} 

declare set_level_shifter_strategy {
  {-rule               Enum         {all low_to_high high_to_low}}
 } {param(-rule)}

declare set_level_shifter_threshold {
  {-voltage            Float   {1}  }
  {-percent            Float   {1}  }
 } {param(-voltage)} 



 declare set_min_porosity {
 {porosity_value       Float        {$par>=0 && $par <=90}}
 {object_list          List          }
 } {param(porosity_value) && param(object_list)}

# sdc 2.0 new constructs
 declare set_min_pulse_width {
  {value               Float       {$par>=0} }
  {object_list          List        }
  {-low                 Flag       }
  {-high                Flag       }
 } {param(value)}


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
