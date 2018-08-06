create_clock -period 1 -name blif_clk_net [get_ports blif_clk_net]
set_input_delay 0 -min -rise [get_ports v4] -clock blif_clk_net
set_input_delay 0 -min -fall [get_ports v4] -clock blif_clk_net
set_input_delay 0 -max -rise [get_ports v4] -clock blif_clk_net
set_input_delay 0 -max -fall [get_ports v4] -clock blif_clk_net
set_input_transition 5 -min -rise [get_ports v4] -clock blif_clk_net
set_input_transition 5 -min -fall [get_ports v4] -clock blif_clk_net
set_input_transition 5 -max -rise [get_ports v4] -clock blif_clk_net
set_input_transition 5 -max -fall [get_ports v4] -clock blif_clk_net
set_input_delay 0 -min -rise [get_ports v3] -clock blif_clk_net
set_input_delay 0 -min -fall [get_ports v3] -clock blif_clk_net
set_input_delay 0 -max -rise [get_ports v3] -clock blif_clk_net
set_input_delay 0 -max -fall [get_ports v3] -clock blif_clk_net
set_input_transition 5 -min -rise [get_ports v3] -clock blif_clk_net
set_input_transition 5 -min -fall [get_ports v3] -clock blif_clk_net
set_input_transition 5 -max -rise [get_ports v3] -clock blif_clk_net
set_input_transition 5 -max -fall [get_ports v3] -clock blif_clk_net
set_input_delay 0 -min -rise [get_ports CLR] -clock blif_clk_net
set_input_delay 0 -min -fall [get_ports CLR] -clock blif_clk_net
set_input_delay 0 -max -rise [get_ports CLR] -clock blif_clk_net
set_input_delay 0 -max -fall [get_ports CLR] -clock blif_clk_net
set_input_transition 5 -min -rise [get_ports CLR] -clock blif_clk_net
set_input_transition 5 -min -fall [get_ports CLR] -clock blif_clk_net
set_input_transition 5 -max -rise [get_ports CLR] -clock blif_clk_net
set_input_transition 5 -max -fall [get_ports CLR] -clock blif_clk_net
set_input_delay 0 -min -rise [get_ports v5] -clock blif_clk_net
set_input_delay 0 -min -fall [get_ports v5] -clock blif_clk_net
set_input_delay 0 -max -rise [get_ports v5] -clock blif_clk_net
set_input_delay 0 -max -fall [get_ports v5] -clock blif_clk_net
set_input_transition 5 -min -rise [get_ports v5] -clock blif_clk_net
set_input_transition 5 -min -fall [get_ports v5] -clock blif_clk_net
set_input_transition 5 -max -rise [get_ports v5] -clock blif_clk_net
set_input_transition 5 -max -fall [get_ports v5] -clock blif_clk_net
set_input_delay 0 -min -rise [get_ports v1] -clock blif_clk_net
set_input_delay 0 -min -fall [get_ports v1] -clock blif_clk_net
set_input_delay 0 -max -rise [get_ports v1] -clock blif_clk_net
set_input_delay 0 -max -fall [get_ports v1] -clock blif_clk_net
set_input_transition 5 -min -rise [get_ports v1] -clock blif_clk_net
set_input_transition 5 -min -fall [get_ports v1] -clock blif_clk_net
set_input_transition 5 -max -rise [get_ports v1] -clock blif_clk_net
set_input_transition 5 -max -fall [get_ports v1] -clock blif_clk_net
set_input_delay 0 -min -rise [get_ports v0] -clock blif_clk_net
set_input_delay 0 -min -fall [get_ports v0] -clock blif_clk_net
set_input_delay 0 -max -rise [get_ports v0] -clock blif_clk_net
set_input_delay 0 -max -fall [get_ports v0] -clock blif_clk_net
set_input_transition 5 -min -rise [get_ports v0] -clock blif_clk_net
set_input_transition 5 -min -fall [get_ports v0] -clock blif_clk_net
set_input_transition 5 -max -rise [get_ports v0] -clock blif_clk_net
set_input_transition 5 -max -fall [get_ports v0] -clock blif_clk_net
set_input_delay 0 -min -rise [get_ports blif_clk_net] -clock blif_clk_net
set_input_delay 0 -min -fall [get_ports blif_clk_net] -clock blif_clk_net
set_input_delay 0 -max -rise [get_ports blif_clk_net] -clock blif_clk_net
set_input_delay 0 -max -fall [get_ports blif_clk_net] -clock blif_clk_net
set_input_transition 5 -min -rise [get_ports blif_clk_net] -clock blif_clk_net
set_input_transition 5 -min -fall [get_ports blif_clk_net] -clock blif_clk_net
set_input_transition 5 -max -rise [get_ports blif_clk_net] -clock blif_clk_net
set_input_transition 5 -max -fall [get_ports blif_clk_net] -clock blif_clk_net
set_input_delay 0 -min -rise [get_ports v2] -clock blif_clk_net
set_input_delay 0 -min -fall [get_ports v2] -clock blif_clk_net
set_input_delay 0 -max -rise [get_ports v2] -clock blif_clk_net
set_input_delay 0 -max -fall [get_ports v2] -clock blif_clk_net
set_input_transition 5 -min -rise [get_ports v2] -clock blif_clk_net
set_input_transition 5 -min -fall [get_ports v2] -clock blif_clk_net
set_input_transition 5 -max -rise [get_ports v2] -clock blif_clk_net
set_input_transition 5 -max -fall [get_ports v2] -clock blif_clk_net
set_input_delay 0 -min -rise [get_ports v6] -clock blif_clk_net
set_input_delay 0 -min -fall [get_ports v6] -clock blif_clk_net
set_input_delay 0 -max -rise [get_ports v6] -clock blif_clk_net
set_input_delay 0 -max -fall [get_ports v6] -clock blif_clk_net
set_input_transition 5 -min -rise [get_ports v6] -clock blif_clk_net
set_input_transition 5 -min -fall [get_ports v6] -clock blif_clk_net
set_input_transition 5 -max -rise [get_ports v6] -clock blif_clk_net
set_input_transition 5 -max -fall [get_ports v6] -clock blif_clk_net
set_input_delay 0 -min -rise [get_ports blif_reset_net] -clock blif_clk_net
set_input_delay 0 -min -fall [get_ports blif_reset_net] -clock blif_clk_net
set_input_delay 0 -max -rise [get_ports blif_reset_net] -clock blif_clk_net
set_input_delay 0 -max -fall [get_ports blif_reset_net] -clock blif_clk_net
set_input_transition 5 -min -rise [get_ports blif_reset_net] -clock blif_clk_net
set_input_transition 5 -min -fall [get_ports blif_reset_net] -clock blif_clk_net
set_input_transition 5 -max -rise [get_ports blif_reset_net] -clock blif_clk_net
set_input_transition 5 -max -fall [get_ports blif_reset_net] -clock blif_clk_net
set_output_delay -2.1 -min -rise [get_ports v13_D_7] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports v13_D_7] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports v13_D_7] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports v13_D_7] -clock blif_clk_net
set_load -pin_load 4 [get_ports v13_D_7]
set_output_delay -2.1 -min -rise [get_ports v13_D_15] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports v13_D_15] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports v13_D_15] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports v13_D_15] -clock blif_clk_net
set_load -pin_load 4 [get_ports v13_D_15]
set_output_delay -2.1 -min -rise [get_ports v13_D_11] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports v13_D_11] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports v13_D_11] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports v13_D_11] -clock blif_clk_net
set_load -pin_load 4 [get_ports v13_D_11]
set_output_delay -2.1 -min -rise [get_ports v13_D_23] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports v13_D_23] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports v13_D_23] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports v13_D_23] -clock blif_clk_net
set_load -pin_load 4 [get_ports v13_D_23]
set_output_delay -2.1 -min -rise [get_ports v13_D_17] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports v13_D_17] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports v13_D_17] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports v13_D_17] -clock blif_clk_net
set_load -pin_load 4 [get_ports v13_D_17]
set_output_delay -2.1 -min -rise [get_ports v13_D_9] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports v13_D_9] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports v13_D_9] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports v13_D_9] -clock blif_clk_net
set_load -pin_load 4 [get_ports v13_D_9]
set_output_delay -2.1 -min -rise [get_ports v13_D_22] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports v13_D_22] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports v13_D_22] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports v13_D_22] -clock blif_clk_net
set_load -pin_load 4 [get_ports v13_D_22]
set_output_delay -2.1 -min -rise [get_ports v13_D_20] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports v13_D_20] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports v13_D_20] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports v13_D_20] -clock blif_clk_net
set_load -pin_load 4 [get_ports v13_D_20]
set_output_delay -2.1 -min -rise [get_ports v13_D_19] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports v13_D_19] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports v13_D_19] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports v13_D_19] -clock blif_clk_net
set_load -pin_load 4 [get_ports v13_D_19]
set_output_delay -2.1 -min -rise [get_ports v13_D_12] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports v13_D_12] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports v13_D_12] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports v13_D_12] -clock blif_clk_net
set_load -pin_load 4 [get_ports v13_D_12]
set_output_delay -2.1 -min -rise [get_ports v13_D_18] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports v13_D_18] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports v13_D_18] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports v13_D_18] -clock blif_clk_net
set_load -pin_load 4 [get_ports v13_D_18]
set_output_delay -2.1 -min -rise [get_ports v13_D_8] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports v13_D_8] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports v13_D_8] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports v13_D_8] -clock blif_clk_net
set_load -pin_load 4 [get_ports v13_D_8]
set_output_delay -2.1 -min -rise [get_ports v13_D_21] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports v13_D_21] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports v13_D_21] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports v13_D_21] -clock blif_clk_net
set_load -pin_load 4 [get_ports v13_D_21]
set_output_delay -2.1 -min -rise [get_ports v13_D_6] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports v13_D_6] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports v13_D_6] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports v13_D_6] -clock blif_clk_net
set_load -pin_load 4 [get_ports v13_D_6]
set_output_delay -2.1 -min -rise [get_ports v13_D_13] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports v13_D_13] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports v13_D_13] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports v13_D_13] -clock blif_clk_net
set_load -pin_load 4 [get_ports v13_D_13]
set_output_delay -2.1 -min -rise [get_ports v13_D_10] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports v13_D_10] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports v13_D_10] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports v13_D_10] -clock blif_clk_net
set_load -pin_load 4 [get_ports v13_D_10]
set_output_delay -2.1 -min -rise [get_ports v13_D_24] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports v13_D_24] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports v13_D_24] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports v13_D_24] -clock blif_clk_net
set_load -pin_load 4 [get_ports v13_D_24]
set_output_delay -2.1 -min -rise [get_ports v13_D_14] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports v13_D_14] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports v13_D_14] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports v13_D_14] -clock blif_clk_net
set_load -pin_load 4 [get_ports v13_D_14]
set_output_delay -2.1 -min -rise [get_ports v13_D_16] -clock blif_clk_net
set_output_delay -2.1 -min -fall [get_ports v13_D_16] -clock blif_clk_net
set_output_delay -1.2 -max -rise [get_ports v13_D_16] -clock blif_clk_net
set_output_delay -1.2 -max -fall [get_ports v13_D_16] -clock blif_clk_net
set_load -pin_load 4 [get_ports v13_D_16]
