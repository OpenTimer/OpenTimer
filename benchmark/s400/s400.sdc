create_clock -period 1 -name blif_clk_net [get_ports blif_clk_net]
set_input_delay 0 -min -rise [get_ports TEST] -clock blif_clk_net
set_input_delay 0 -min -fall [get_ports TEST] -clock blif_clk_net
set_input_delay 0 -max -rise [get_ports TEST] -clock blif_clk_net
set_input_delay 0 -max -fall [get_ports TEST] -clock blif_clk_net
set_input_transition 5 -min -rise [get_ports TEST] -clock blif_clk_net
set_input_transition 5 -min -fall [get_ports TEST] -clock blif_clk_net
set_input_transition 5 -max -rise [get_ports TEST] -clock blif_clk_net
set_input_transition 5 -max -fall [get_ports TEST] -clock blif_clk_net
set_input_delay 0 -min -rise [get_ports FM] -clock blif_clk_net
set_input_delay 0 -min -fall [get_ports FM] -clock blif_clk_net
set_input_delay 0 -max -rise [get_ports FM] -clock blif_clk_net
set_input_delay 0 -max -fall [get_ports FM] -clock blif_clk_net
set_input_transition 5 -min -rise [get_ports FM] -clock blif_clk_net
set_input_transition 5 -min -fall [get_ports FM] -clock blif_clk_net
set_input_transition 5 -max -rise [get_ports FM] -clock blif_clk_net
set_input_transition 5 -max -fall [get_ports FM] -clock blif_clk_net
set_input_delay 0 -min -rise [get_ports blif_clk_net] -clock blif_clk_net
set_input_delay 0 -min -fall [get_ports blif_clk_net] -clock blif_clk_net
set_input_delay 0 -max -rise [get_ports blif_clk_net] -clock blif_clk_net
set_input_delay 0 -max -fall [get_ports blif_clk_net] -clock blif_clk_net
set_input_transition 5 -min -rise [get_ports blif_clk_net] -clock blif_clk_net
set_input_transition 5 -min -fall [get_ports blif_clk_net] -clock blif_clk_net
set_input_transition 5 -max -rise [get_ports blif_clk_net] -clock blif_clk_net
set_input_transition 5 -max -fall [get_ports blif_clk_net] -clock blif_clk_net
set_input_delay 0 -min -rise [get_ports CLR] -clock blif_clk_net
set_input_delay 0 -min -fall [get_ports CLR] -clock blif_clk_net
set_input_delay 0 -max -rise [get_ports CLR] -clock blif_clk_net
set_input_delay 0 -max -fall [get_ports CLR] -clock blif_clk_net
set_input_transition 5 -min -rise [get_ports CLR] -clock blif_clk_net
set_input_transition 5 -min -fall [get_ports CLR] -clock blif_clk_net
set_input_transition 5 -max -rise [get_ports CLR] -clock blif_clk_net
set_input_transition 5 -max -fall [get_ports CLR] -clock blif_clk_net
set_input_delay 0 -min -rise [get_ports blif_reset_net] -clock blif_clk_net
set_input_delay 0 -min -fall [get_ports blif_reset_net] -clock blif_clk_net
set_input_delay 0 -max -rise [get_ports blif_reset_net] -clock blif_clk_net
set_input_delay 0 -max -fall [get_ports blif_reset_net] -clock blif_clk_net
set_input_transition 5 -min -rise [get_ports blif_reset_net] -clock blif_clk_net
set_input_transition 5 -min -fall [get_ports blif_reset_net] -clock blif_clk_net
set_input_transition 5 -max -rise [get_ports blif_reset_net] -clock blif_clk_net
set_input_transition 5 -max -fall [get_ports blif_reset_net] -clock blif_clk_net
set_output_delay -2.1 -min -rise [get_ports YLW1] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports YLW1] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports YLW1] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports YLW1] -clock blif_clk_net
set_load -pin_load 4 [get_ports YLW1]
set_output_delay -2.1 -min -rise [get_ports RED2] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports RED2] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports RED2] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports RED2] -clock blif_clk_net
set_load -pin_load 4 [get_ports RED2]
set_output_delay -2.1 -min -rise [get_ports GRN1] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports GRN1] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports GRN1] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports GRN1] -clock blif_clk_net
set_load -pin_load 4 [get_ports GRN1]
set_output_delay -2.1 -min -rise [get_ports RED1] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports RED1] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports RED1] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports RED1] -clock blif_clk_net
set_load -pin_load 4 [get_ports RED1]
set_output_delay -2.1 -min -rise [get_ports YLW2] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports YLW2] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports YLW2] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports YLW2] -clock blif_clk_net
set_load -pin_load 4 [get_ports YLW2]
set_output_delay -2.1 -min -rise [get_ports GRN2] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports GRN2] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports GRN2] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports GRN2] -clock blif_clk_net
set_load -pin_load 4 [get_ports GRN2]
