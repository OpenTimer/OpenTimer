set clock_cycle 400
set uncertainty 0
set io_delay 0

set clock_port clk

create_clock -name clk -period 400 [get_ports clk]

set_input_delay -clock [get_clocks clk] -add_delay -max 0 [all_inputs]
set_output_delay -clock [get_clocks clk] -add_delay -max 0 [all_outputs]
