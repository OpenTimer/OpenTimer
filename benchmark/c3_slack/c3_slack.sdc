set_input_delay 0 -min -rise [get_ports nx1]
set_input_delay 0 -min -fall [get_ports nx1]
set_input_delay 0 -max -rise [get_ports nx1]
set_input_delay 0 -max -fall [get_ports nx1]
set_input_transition 5 -min -rise [get_ports nx1]
set_input_transition 5 -min -fall [get_ports nx1]
set_input_transition 5 -max -rise [get_ports nx1]
set_input_transition 5 -max -fall [get_ports nx1]
set_input_delay 0 -min -rise [get_ports nx3]
set_input_delay 0 -min -fall [get_ports nx3]
set_input_delay 0 -max -rise [get_ports nx3]
set_input_delay 0 -max -fall [get_ports nx3]
set_input_transition 5 -min -rise [get_ports nx3]
set_input_transition 5 -min -fall [get_ports nx3]
set_input_transition 5 -max -rise [get_ports nx3]
set_input_transition 5 -max -fall [get_ports nx3]
set_input_delay 0 -min -rise [get_ports nx2]
set_input_delay 0 -min -fall [get_ports nx2]
set_input_delay 0 -max -rise [get_ports nx2]
set_input_delay 0 -max -fall [get_ports nx2]
set_input_transition 10 -min -rise [get_ports nx2]
set_input_transition 15 -min -fall [get_ports nx2]
set_input_transition 10 -max -rise [get_ports nx2]
set_input_transition 15 -max -fall [get_ports nx2]
set_input_delay 0 -min -rise [get_ports nx4]
set_input_delay 0 -min -fall [get_ports nx4]
set_input_delay 0 -max -rise [get_ports nx4]
set_input_delay 0 -max -fall [get_ports nx4]
set_input_transition 5 -min -rise [get_ports nx4]
set_input_transition 5 -min -fall [get_ports nx4]
set_input_transition 5 -max -rise [get_ports nx4]
set_input_transition 5 -max -fall [get_ports nx4]
create_clock -period 100 -name virtual_clock
set_output_delay -9 -min -rise [get_ports nx33] -clock virtual_clock
set_output_delay -9 -min -fall [get_ports nx33] -clock virtual_clock
set_output_delay 89 -max -rise [get_ports nx33] -clock virtual_clock
set_output_delay 89 -max -fall [get_ports nx33] -clock virtual_clock
set_load -pin_load 4 [get_ports nx33]
set_output_delay -9 -min -rise [get_ports nx44] -clock virtual_clock
set_output_delay -9 -min -fall [get_ports nx44] -clock virtual_clock
set_output_delay 89 -max -rise [get_ports nx44] -clock virtual_clock
set_output_delay 89 -max -fall [get_ports nx44] -clock virtual_clock
set_load -pin_load 4 [get_ports nx44]
set_output_delay -9 -min -rise [get_ports nx12] -clock virtual_clock
set_output_delay -9 -min -fall [get_ports nx12] -clock virtual_clock
set_output_delay 89 -max -rise [get_ports nx12] -clock virtual_clock
set_output_delay 89 -max -fall [get_ports nx12] -clock virtual_clock
set_load -pin_load 4 [get_ports nx12]
