module s27 (
G1,
G2,
clk_net,
reset_net,
G3,
G0,
G17);

// Start PIs
input G1;
input G2;
input clk_net;
input reset_net;
input G3;
input G0;

// Start POs
output G17;

// Start wires
wire G1;
wire net_5;
wire net_15;
wire net_27;
wire G17;
wire reset_net;
wire net_14;
wire G3;
wire net_26;
wire clk_net;
wire net_13;
wire G2;
wire net_19;
wire net_3;
wire net_22;
wire net_16;
wire net_6;
wire net_24;
wire net_11;
wire net_1;
wire net_23;
wire net_18;
wire net_12;
wire net_2;
wire net_10;
wire net_8;
wire net_9;
wire net_25;
wire net_21;
wire net_7;
wire net_20;
wire G0;
wire net_4;
wire net_17;

// Start cells
CLKBUF_X2 inst_19 ( .A(net_17), .Z(net_18) );
DFFR_X2 inst_14 ( .RN(net_12), .D(net_10), .QN(net_3), .CK(net_27) );
INV_X1 inst_12 ( .A(net_16), .ZN(G17) );
INV_X4 inst_8 ( .ZN(net_5), .A(net_1) );
NOR2_X4 inst_2 ( .ZN(net_11), .A2(net_9), .A1(net_6) );
NOR2_X4 inst_1 ( .A1(net_14), .ZN(net_8), .A2(G3) );
CLKBUF_X2 inst_21 ( .A(net_19), .Z(net_20) );
CLKBUF_X2 inst_25 ( .A(net_23), .Z(net_24) );
NAND2_X2 inst_7 ( .ZN(net_7), .A1(net_4), .A2(net_3) );
CLKBUF_X2 inst_20 ( .A(net_18), .Z(net_19) );
INV_X1 inst_13 ( .ZN(net_12), .A(reset_net) );
CLKBUF_X2 inst_27 ( .A(net_25), .Z(net_26) );
CLKBUF_X2 inst_26 ( .A(net_17), .Z(net_25) );
NOR3_X4 inst_0 ( .ZN(net_16), .A1(net_11), .A3(net_8), .A2(net_5) );
CLKBUF_X2 inst_18 ( .A(clk_net), .Z(net_17) );
DFFR_X2 inst_15 ( .D(net_16), .RN(net_12), .QN(net_2), .CK(net_19) );
DFFR_X2 inst_16 ( .D(net_13), .RN(net_12), .QN(net_1), .CK(net_24) );
CLKBUF_X2 inst_24 ( .A(net_22), .Z(net_23) );
NOR2_X2 inst_3 ( .ZN(net_14), .A1(net_2), .A2(G0) );
NOR2_X2 inst_6 ( .A1(net_16), .A2(net_15), .ZN(net_13) );
INV_X4 inst_9 ( .ZN(net_9), .A(net_7) );
NOR2_X2 inst_5 ( .ZN(net_10), .A2(net_9), .A1(G2) );
INV_X2 inst_10 ( .ZN(net_4), .A(G1) );
NOR2_X2 inst_4 ( .ZN(net_6), .A1(net_2), .A2(G0) );
CLKBUF_X2 inst_23 ( .A(net_21), .Z(net_22) );
INV_X2 inst_11 ( .ZN(net_15), .A(G0) );
CLKBUF_X2 inst_28 ( .A(net_26), .Z(net_27) );
CLKBUF_X2 inst_22 ( .A(net_20), .Z(net_21) );

endmodule
