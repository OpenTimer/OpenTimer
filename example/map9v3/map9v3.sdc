create_clock -period 10000 -name clock [get_ports clock]
set_input_transition 100 -min -rise [get_ports clock] -clock clock
set_input_transition 100 -min -fall [get_ports clock] -clock clock
set_input_transition 100 -max -rise [get_ports clock] -clock clock
set_input_transition 100 -max -fall [get_ports clock] -clock clock
set_input_delay 1000 -min -rise [get_ports N_0_] -clock clock
set_input_delay 1000 -min -fall [get_ports N_0_] -clock clock
set_input_delay 1000 -max -rise [get_ports N_0_] -clock clock
set_input_delay 1000 -max -fall [get_ports N_0_] -clock clock
set_input_delay 1000 -min -rise [get_ports N_1_] -clock clock
set_input_delay 1000 -min -fall [get_ports N_1_] -clock clock
set_input_delay 1000 -max -rise [get_ports N_1_] -clock clock
set_input_delay 1000 -max -fall [get_ports N_1_] -clock clock
set_input_delay 1000 -min -rise [get_ports N_2_] -clock clock
set_input_delay 1000 -min -fall [get_ports N_2_] -clock clock
set_input_delay 1000 -max -rise [get_ports N_2_] -clock clock
set_input_delay 1000 -max -fall [get_ports N_2_] -clock clock
set_input_delay 1000 -min -rise [get_ports N_3_] -clock clock
set_input_delay 1000 -min -fall [get_ports N_3_] -clock clock
set_input_delay 1000 -max -rise [get_ports N_3_] -clock clock
set_input_delay 1000 -max -fall [get_ports N_3_] -clock clock
set_input_delay 1000 -min -rise [get_ports N_4_] -clock clock
set_input_delay 1000 -min -fall [get_ports N_4_] -clock clock
set_input_delay 1000 -max -rise [get_ports N_4_] -clock clock
set_input_delay 1000 -max -fall [get_ports N_4_] -clock clock
set_input_delay 1000 -min -rise [get_ports N_5_] -clock clock
set_input_delay 1000 -min -fall [get_ports N_5_] -clock clock
set_input_delay 1000 -max -rise [get_ports N_5_] -clock clock
set_input_delay 1000 -max -fall [get_ports N_5_] -clock clock
set_input_delay 1000 -min -rise [get_ports N_6_] -clock clock
set_input_delay 1000 -min -fall [get_ports N_6_] -clock clock
set_input_delay 1000 -max -rise [get_ports N_6_] -clock clock
set_input_delay 1000 -max -fall [get_ports N_6_] -clock clock
set_input_delay 1000 -min -rise [get_ports N_7_] -clock clock
set_input_delay 1000 -min -fall [get_ports N_7_] -clock clock
set_input_delay 1000 -max -rise [get_ports N_7_] -clock clock
set_input_delay 1000 -max -fall [get_ports N_7_] -clock clock
set_input_delay 1000 -min -rise [get_ports N_8_] -clock clock
set_input_delay 1000 -min -fall [get_ports N_8_] -clock clock
set_input_delay 1000 -max -rise [get_ports N_8_] -clock clock
set_input_delay 1000 -max -fall [get_ports N_8_] -clock clock
set_input_delay 1000 -min -rise [get_ports reset] -clock clock
set_input_delay 1000 -min -fall [get_ports reset] -clock clock
set_input_delay 1000 -max -rise [get_ports reset] -clock clock
set_input_delay 1000 -max -fall [get_ports reset] -clock clock
set_input_delay 1000 -min -rise [get_ports start] -clock clock
set_input_delay 1000 -min -fall [get_ports start] -clock clock
set_input_delay 1000 -max -rise [get_ports start] -clock clock
set_input_delay 1000 -max -fall [get_ports start] -clock clock
set_input_transition 100 -min -rise [get_ports N_0_] -clock clock
set_input_transition 100 -min -fall [get_ports N_0_] -clock clock
set_input_transition 100 -max -rise [get_ports N_0_] -clock clock
set_input_transition 100 -max -fall [get_ports N_0_] -clock clock
set_input_transition 100 -min -rise [get_ports N_1_] -clock clock
set_input_transition 100 -min -fall [get_ports N_1_] -clock clock
set_input_transition 100 -max -rise [get_ports N_1_] -clock clock
set_input_transition 100 -max -fall [get_ports N_1_] -clock clock
set_input_transition 100 -min -rise [get_ports N_2_] -clock clock
set_input_transition 100 -min -fall [get_ports N_2_] -clock clock
set_input_transition 100 -max -rise [get_ports N_2_] -clock clock
set_input_transition 100 -max -fall [get_ports N_2_] -clock clock
set_input_transition 100 -min -rise [get_ports N_3_] -clock clock
set_input_transition 100 -min -fall [get_ports N_3_] -clock clock
set_input_transition 100 -max -rise [get_ports N_3_] -clock clock
set_input_transition 100 -max -fall [get_ports N_3_] -clock clock
set_input_transition 100 -min -rise [get_ports N_4_] -clock clock
set_input_transition 100 -min -fall [get_ports N_4_] -clock clock
set_input_transition 100 -max -rise [get_ports N_4_] -clock clock
set_input_transition 100 -max -fall [get_ports N_4_] -clock clock
set_input_transition 100 -min -rise [get_ports N_5_] -clock clock
set_input_transition 100 -min -fall [get_ports N_5_] -clock clock
set_input_transition 100 -max -rise [get_ports N_5_] -clock clock
set_input_transition 100 -max -fall [get_ports N_5_] -clock clock
set_input_transition 100 -min -rise [get_ports N_6_] -clock clock
set_input_transition 100 -min -fall [get_ports N_6_] -clock clock
set_input_transition 100 -max -rise [get_ports N_6_] -clock clock
set_input_transition 100 -max -fall [get_ports N_6_] -clock clock
set_input_transition 100 -min -rise [get_ports N_7_] -clock clock
set_input_transition 100 -min -fall [get_ports N_7_] -clock clock
set_input_transition 100 -max -rise [get_ports N_7_] -clock clock
set_input_transition 100 -max -fall [get_ports N_7_] -clock clock
set_input_transition 100 -min -rise [get_ports N_8_] -clock clock
set_input_transition 100 -min -fall [get_ports N_8_] -clock clock
set_input_transition 100 -max -rise [get_ports N_8_] -clock clock
set_input_transition 100 -max -fall [get_ports N_8_] -clock clock
set_input_transition 100 -min -rise [get_ports reset] -clock clock
set_input_transition 100 -min -fall [get_ports reset] -clock clock
set_input_transition 100 -max -rise [get_ports reset] -clock clock
set_input_transition 100 -max -fall [get_ports reset] -clock clock
set_input_transition 100 -min -rise [get_ports start] -clock clock
set_input_transition 100 -min -fall [get_ports start] -clock clock
set_input_transition 100 -max -rise [get_ports start] -clock clock
set_input_transition 100 -max -fall [get_ports start] -clock clock
set_output_delay -1000 -min -rise [get_ports counter_0_] -clock clock
set_output_delay -1000 -min -fall [get_ports counter_0_] -clock clock
set_output_delay 9000 -max -rise [get_ports counter_0_] -clock clock
set_output_delay 9000 -max -fall [get_ports counter_0_] -clock clock
set_output_delay -1000 -min -rise [get_ports counter_1_] -clock clock
set_output_delay -1000 -min -fall [get_ports counter_1_] -clock clock
set_output_delay 9000 -max -rise [get_ports counter_1_] -clock clock
set_output_delay 9000 -max -fall [get_ports counter_1_] -clock clock
set_output_delay -1000 -min -rise [get_ports counter_2_] -clock clock
set_output_delay -1000 -min -fall [get_ports counter_2_] -clock clock
set_output_delay 9000 -max -rise [get_ports counter_2_] -clock clock
set_output_delay 9000 -max -fall [get_ports counter_2_] -clock clock
set_output_delay -1000 -min -rise [get_ports counter_3_] -clock clock
set_output_delay -1000 -min -fall [get_ports counter_3_] -clock clock
set_output_delay 9000 -max -rise [get_ports counter_3_] -clock clock
set_output_delay 9000 -max -fall [get_ports counter_3_] -clock clock
set_output_delay -1000 -min -rise [get_ports counter_4_] -clock clock
set_output_delay -1000 -min -fall [get_ports counter_4_] -clock clock
set_output_delay 9000 -max -rise [get_ports counter_4_] -clock clock
set_output_delay 9000 -max -fall [get_ports counter_4_] -clock clock
set_output_delay -1000 -min -rise [get_ports counter_5_] -clock clock
set_output_delay -1000 -min -fall [get_ports counter_5_] -clock clock
set_output_delay 9000 -max -rise [get_ports counter_5_] -clock clock
set_output_delay 9000 -max -fall [get_ports counter_5_] -clock clock
set_output_delay -1000 -min -rise [get_ports counter_6_] -clock clock
set_output_delay -1000 -min -fall [get_ports counter_6_] -clock clock
set_output_delay 9000 -max -rise [get_ports counter_6_] -clock clock
set_output_delay 9000 -max -fall [get_ports counter_6_] -clock clock
set_output_delay -1000 -min -rise [get_ports counter_7_] -clock clock
set_output_delay -1000 -min -fall [get_ports counter_7_] -clock clock
set_output_delay 9000 -max -rise [get_ports counter_7_] -clock clock
set_output_delay 9000 -max -fall [get_ports counter_7_] -clock clock
set_output_delay -1000 -min -rise [get_ports done] -clock clock
set_output_delay -1000 -min -fall [get_ports done] -clock clock
set_output_delay 9000 -max -rise [get_ports done] -clock clock
set_output_delay 9000 -max -fall [get_ports done] -clock clock
set_output_delay -1000 -min -rise [get_ports dp_0_] -clock clock
set_output_delay -1000 -min -fall [get_ports dp_0_] -clock clock
set_output_delay 9000 -max -rise [get_ports dp_0_] -clock clock
set_output_delay 9000 -max -fall [get_ports dp_0_] -clock clock
set_output_delay -1000 -min -rise [get_ports dp_1_] -clock clock
set_output_delay -1000 -min -fall [get_ports dp_1_] -clock clock
set_output_delay 9000 -max -rise [get_ports dp_1_] -clock clock
set_output_delay 9000 -max -fall [get_ports dp_1_] -clock clock
set_output_delay -1000 -min -rise [get_ports dp_2_] -clock clock
set_output_delay -1000 -min -fall [get_ports dp_2_] -clock clock
set_output_delay 9000 -max -rise [get_ports dp_2_] -clock clock
set_output_delay 9000 -max -fall [get_ports dp_2_] -clock clock
set_output_delay -1000 -min -rise [get_ports dp_3_] -clock clock
set_output_delay -1000 -min -fall [get_ports dp_3_] -clock clock
set_output_delay 9000 -max -rise [get_ports dp_3_] -clock clock
set_output_delay 9000 -max -fall [get_ports dp_3_] -clock clock
set_output_delay -1000 -min -rise [get_ports dp_4_] -clock clock
set_output_delay -1000 -min -fall [get_ports dp_4_] -clock clock
set_output_delay 9000 -max -rise [get_ports dp_4_] -clock clock
set_output_delay 9000 -max -fall [get_ports dp_4_] -clock clock
set_output_delay -1000 -min -rise [get_ports dp_5_] -clock clock
set_output_delay -1000 -min -fall [get_ports dp_5_] -clock clock
set_output_delay 9000 -max -rise [get_ports dp_5_] -clock clock
set_output_delay 9000 -max -fall [get_ports dp_5_] -clock clock
set_output_delay -1000 -min -rise [get_ports dp_6_] -clock clock
set_output_delay -1000 -min -fall [get_ports dp_6_] -clock clock
set_output_delay 9000 -max -rise [get_ports dp_6_] -clock clock
set_output_delay 9000 -max -fall [get_ports dp_6_] -clock clock
set_output_delay -1000 -min -rise [get_ports dp_7_] -clock clock
set_output_delay -1000 -min -fall [get_ports dp_7_] -clock clock
set_output_delay 9000 -max -rise [get_ports dp_7_] -clock clock
set_output_delay 9000 -max -fall [get_ports dp_7_] -clock clock
set_output_delay -1000 -min -rise [get_ports dp_8_] -clock clock
set_output_delay -1000 -min -fall [get_ports dp_8_] -clock clock
set_output_delay 9000 -max -rise [get_ports dp_8_] -clock clock
set_output_delay 9000 -max -fall [get_ports dp_8_] -clock clock
set_output_delay -1000 -min -rise [get_ports sr_0_] -clock clock
set_output_delay -1000 -min -fall [get_ports sr_0_] -clock clock
set_output_delay 9000 -max -rise [get_ports sr_0_] -clock clock
set_output_delay 9000 -max -fall [get_ports sr_0_] -clock clock
set_output_delay -1000 -min -rise [get_ports sr_1_] -clock clock
set_output_delay -1000 -min -fall [get_ports sr_1_] -clock clock
set_output_delay 9000 -max -rise [get_ports sr_1_] -clock clock
set_output_delay 9000 -max -fall [get_ports sr_1_] -clock clock
set_output_delay -1000 -min -rise [get_ports sr_2_] -clock clock
set_output_delay -1000 -min -fall [get_ports sr_2_] -clock clock
set_output_delay 9000 -max -rise [get_ports sr_2_] -clock clock
set_output_delay 9000 -max -fall [get_ports sr_2_] -clock clock
set_output_delay -1000 -min -rise [get_ports sr_3_] -clock clock
set_output_delay -1000 -min -fall [get_ports sr_3_] -clock clock
set_output_delay 9000 -max -rise [get_ports sr_3_] -clock clock
set_output_delay 9000 -max -fall [get_ports sr_3_] -clock clock
set_output_delay -1000 -min -rise [get_ports sr_4_] -clock clock
set_output_delay -1000 -min -fall [get_ports sr_4_] -clock clock
set_output_delay 9000 -max -rise [get_ports sr_4_] -clock clock
set_output_delay 9000 -max -fall [get_ports sr_4_] -clock clock
set_output_delay -1000 -min -rise [get_ports sr_5_] -clock clock
set_output_delay -1000 -min -fall [get_ports sr_5_] -clock clock
set_output_delay 9000 -max -rise [get_ports sr_5_] -clock clock
set_output_delay 9000 -max -fall [get_ports sr_5_] -clock clock
set_output_delay -1000 -min -rise [get_ports sr_6_] -clock clock
set_output_delay -1000 -min -fall [get_ports sr_6_] -clock clock
set_output_delay 9000 -max -rise [get_ports sr_6_] -clock clock
set_output_delay 9000 -max -fall [get_ports sr_6_] -clock clock
set_output_delay -1000 -min -rise [get_ports sr_7_] -clock clock
set_output_delay -1000 -min -fall [get_ports sr_7_] -clock clock
set_output_delay 9000 -max -rise [get_ports sr_7_] -clock clock
set_output_delay 9000 -max -fall [get_ports sr_7_] -clock clock
set_load -pin_load 10 [get_ports counter_0_]
set_load -pin_load 10 [get_ports counter_1_]
set_load -pin_load 10 [get_ports counter_2_]
set_load -pin_load 10 [get_ports counter_3_]
set_load -pin_load 10 [get_ports counter_4_]
set_load -pin_load 10 [get_ports counter_5_]
set_load -pin_load 10 [get_ports counter_6_]
set_load -pin_load 10 [get_ports counter_7_]
set_load -pin_load 10 [get_ports done]
set_load -pin_load 10 [get_ports dp_0_]
set_load -pin_load 10 [get_ports dp_1_]
set_load -pin_load 10 [get_ports dp_2_]
set_load -pin_load 10 [get_ports dp_3_]
set_load -pin_load 10 [get_ports dp_4_]
set_load -pin_load 10 [get_ports dp_5_]
set_load -pin_load 10 [get_ports dp_6_]
set_load -pin_load 10 [get_ports dp_7_]
set_load -pin_load 10 [get_ports dp_8_]
set_load -pin_load 10 [get_ports sr_0_]
set_load -pin_load 10 [get_ports sr_1_]
set_load -pin_load 10 [get_ports sr_2_]
set_load -pin_load 10 [get_ports sr_3_]
set_load -pin_load 10 [get_ports sr_4_]
set_load -pin_load 10 [get_ports sr_5_]
set_load -pin_load 10 [get_ports sr_6_]
set_load -pin_load 10 [get_ports sr_7_]
