module c3_path (
nx1,
nx3,
nx2,
nx4,
nx33,
nx44,
nx12);

// Start PIs
input nx1;
input nx3;
input nx2;
input nx4;

// Start POs
output nx33;
output nx44;
output nx12;

// Start wires
wire nx1;
wire nx3;
wire nx33;
wire nx44;
wire nx12;
wire nx2;
wire nx4;

// Start cells
BUF_X1 inst_2 ( .Z(nx44), .A(nx4) );
INV_X1 inst_1 ( .ZN(nx33), .A(nx3) );
NAND2_X1 inst_0 ( .ZN(nx12), .A2(nx2), .A1(nx1) );

endmodule
