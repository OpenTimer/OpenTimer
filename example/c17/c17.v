module c17 (
nx1,
nx7,
nx3,
nx2,
nx6,
nx23,
nx22);

// Start PIs
input nx1;
input nx7;
input nx3;
input nx2;
input nx6;

// Start POs
output nx23;
output nx22;

// Start wires
wire net_1;
wire nx23;
wire nx1;
wire nx7;
wire nx3;
wire net_2;
wire nx22;
wire nx6;
wire net_0;
wire net_3;
wire nx2;

// Start cells
NAND2_X1 inst_5 ( .A2(net_3), .A1(net_0), .ZN(nx22) );
NAND2_X1 inst_2 ( .ZN(net_2), .A2(net_1), .A1(nx7) );
NAND2_X1 inst_1 ( .ZN(net_0), .A2(nx3), .A1(nx1) );
NAND2_X1 inst_4 ( .A1(net_3), .A2(net_2), .ZN(nx23) );
NAND2_X1 inst_3 ( .ZN(net_3), .A2(net_1), .A1(nx2) );
NAND2_X1 inst_0 ( .ZN(net_1), .A2(nx6), .A1(nx3) );

endmodule
