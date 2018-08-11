create_clock -period 1 -name blif_clk_net [get_ports blif_clk_net]
set_input_delay 0 -min -rise [get_ports G1] -clock blif_clk_net
set_input_delay 0 -min -fall [get_ports G1] -clock blif_clk_net
set_input_delay 0 -max -rise [get_ports G1] -clock blif_clk_net
set_input_delay 0 -max -fall [get_ports G1] -clock blif_clk_net
set_input_transition 5 -min -rise [get_ports G1] -clock blif_clk_net
set_input_transition 5 -min -fall [get_ports G1] -clock blif_clk_net
set_input_transition 5 -max -rise [get_ports G1] -clock blif_clk_net
set_input_transition 5 -max -fall [get_ports G1] -clock blif_clk_net
set_input_delay 0 -min -rise [get_ports G2] -clock blif_clk_net
set_input_delay 0 -min -fall [get_ports G2] -clock blif_clk_net
set_input_delay 0 -max -rise [get_ports G2] -clock blif_clk_net
set_input_delay 0 -max -fall [get_ports G2] -clock blif_clk_net
set_input_transition 5 -min -rise [get_ports G2] -clock blif_clk_net
set_input_transition 5 -min -fall [get_ports G2] -clock blif_clk_net
set_input_transition 5 -max -rise [get_ports G2] -clock blif_clk_net
set_input_transition 5 -max -fall [get_ports G2] -clock blif_clk_net
set_input_delay 0 -min -rise [get_ports blif_clk_net] -clock blif_clk_net
set_input_delay 0 -min -fall [get_ports blif_clk_net] -clock blif_clk_net
set_input_delay 0 -max -rise [get_ports blif_clk_net] -clock blif_clk_net
set_input_delay 0 -max -fall [get_ports blif_clk_net] -clock blif_clk_net
set_input_transition 5 -min -rise [get_ports blif_clk_net] -clock blif_clk_net
set_input_transition 5 -min -fall [get_ports blif_clk_net] -clock blif_clk_net
set_input_transition 5 -max -rise [get_ports blif_clk_net] -clock blif_clk_net
set_input_transition 5 -max -fall [get_ports blif_clk_net] -clock blif_clk_net
set_input_delay 0 -min -rise [get_ports blif_reset_net] -clock blif_clk_net
set_input_delay 0 -min -fall [get_ports blif_reset_net] -clock blif_clk_net
set_input_delay 0 -max -rise [get_ports blif_reset_net] -clock blif_clk_net
set_input_delay 0 -max -fall [get_ports blif_reset_net] -clock blif_clk_net
set_input_transition 5 -min -rise [get_ports blif_reset_net] -clock blif_clk_net
set_input_transition 5 -min -fall [get_ports blif_reset_net] -clock blif_clk_net
set_input_transition 5 -max -rise [get_ports blif_reset_net] -clock blif_clk_net
set_input_transition 5 -max -fall [get_ports blif_reset_net] -clock blif_clk_net
set_input_delay 0 -min -rise [get_ports G0] -clock blif_clk_net
set_input_delay 0 -min -fall [get_ports G0] -clock blif_clk_net
set_input_delay 0 -max -rise [get_ports G0] -clock blif_clk_net
set_input_delay 0 -max -fall [get_ports G0] -clock blif_clk_net
set_input_transition 5 -min -rise [get_ports G0] -clock blif_clk_net
set_input_transition 5 -min -fall [get_ports G0] -clock blif_clk_net
set_input_transition 5 -max -rise [get_ports G0] -clock blif_clk_net
set_input_transition 5 -max -fall [get_ports G0] -clock blif_clk_net
set_output_delay -2.1 -min -rise [get_ports G198] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports G198] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports G198] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports G198] -clock blif_clk_net
set_load -pin_load 4 [get_ports G198]
set_output_delay -2.1 -min -rise [get_ports G213] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports G213] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports G213] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports G213] -clock blif_clk_net
set_load -pin_load 4 [get_ports G213]
set_output_delay -2.1 -min -rise [get_ports G148] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports G148] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports G148] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports G148] -clock blif_clk_net
set_load -pin_load 4 [get_ports G148]
set_output_delay -2.1 -min -rise [get_ports G214] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports G214] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports G214] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports G214] -clock blif_clk_net
set_load -pin_load 4 [get_ports G214]
set_output_delay -2.1 -min -rise [get_ports G199] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports G199] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports G199] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports G199] -clock blif_clk_net
set_load -pin_load 4 [get_ports G199]
set_output_delay -2.1 -min -rise [get_ports G147] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports G147] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports G147] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports G147] -clock blif_clk_net
set_load -pin_load 4 [get_ports G147]
