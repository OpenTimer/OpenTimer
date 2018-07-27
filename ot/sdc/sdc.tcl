#!/usr/bin/env tclsh

source [file join [file dirname [info script]] sdcparsercore.tcl]
lappend auto_path [file join json]

package require json 
package require json::write

# disable json indentation
json::write indented false

# environment variable
#global env
#puts $env(PATH)

# Namespace ot ----------------------------------------------------------------
namespace eval ot {

# sdcjson
variable sdc_json [list]

# callback procedure
proc sdc_callback {cmd parsing_result} {

  variable sdc_json

  # put reference to data structure after parsing
  upvar $parsing_result res
  
  # Switch on cmd type
  switch -- $cmd {

    create_clock -
    set_input_delay -
    set_input_transition -
    set_output_delay -
    set_clock_uncertainty -
    set_load -
    set_units { 

      set cmd_body [list "command" [json::write string $cmd]]

      foreach {key value} [array get res] {
        lappend cmd_body $key [json::write string $value]
      }

      #puts stdout [json::write object {*}$cmd_body]

      lappend sdc_json [json::write object {*}$cmd_body]

      return ""
    }

    get_clocks -
    get_ports {
      return [join $res(patterns) " "]
    }

    all_inputs {
      return "all_inputs"
    }

    all_clocks {
      return "all_clocks"
    }

    all_outputs {
      return "all_outputs"
    }

    default {
      puts stderr "$cmd not supported yet"
    }
  }
}

}  
# end of ot namespace ---------------------------------------------------------

# main program

sdc::register_callback ot::sdc_callback
sdc::parse_file [lindex $argv 0]

set json_output [open [lindex $argv 1] w]

#puts stdout [json::write array {*}$ot::sdc_json]
puts -nonewline $json_output [json::write array {*}$ot::sdc_json]

close $json_output


