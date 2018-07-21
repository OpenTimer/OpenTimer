module c432 (
n43gat,
n17gat,
n34gat,
n27gat,
n82gat,
n99gat,
n21gat,
n66gat,
n102gat,
n47gat,
n92gat,
n14gat,
n95gat,
n105gat,
n30gat,
n1gat,
n40gat,
n37gat,
n4gat,
n112gat,
n76gat,
n56gat,
n115gat,
n53gat,
n86gat,
n69gat,
n8gat,
n79gat,
n73gat,
n11gat,
n60gat,
n50gat,
n108gat,
n63gat,
n24gat,
n89gat,
n432gat,
n430gat,
n421gat,
n370gat,
n329gat,
n223gat,
n431gat);

// Start PIs
input n43gat;
input n17gat;
input n34gat;
input n27gat;
input n82gat;
input n99gat;
input n21gat;
input n66gat;
input n102gat;
input n47gat;
input n92gat;
input n14gat;
input n95gat;
input n105gat;
input n30gat;
input n1gat;
input n40gat;
input n37gat;
input n4gat;
input n112gat;
input n76gat;
input n56gat;
input n115gat;
input n53gat;
input n86gat;
input n69gat;
input n8gat;
input n79gat;
input n73gat;
input n11gat;
input n60gat;
input n50gat;
input n108gat;
input n63gat;
input n24gat;
input n89gat;

// Start POs
output n432gat;
output n430gat;
output n421gat;
output n370gat;
output n329gat;
output n223gat;
output n431gat;

// Start wires
wire n43gat;
wire net_107;
wire net_47;
wire n34gat;
wire n82gat;
wire n99gat;
wire net_61;
wire net_54;
wire n66gat;
wire net_105;
wire net_62;
wire net_6;
wire net_119;
wire net_98;
wire net_23;
wire net_117;
wire net_12;
wire n102gat;
wire net_74;
wire net_53;
wire net_93;
wire n105gat;
wire n30gat;
wire n40gat;
wire n37gat;
wire n112gat;
wire n432gat;
wire n56gat;
wire net_14;
wire net_113;
wire net_26;
wire net_76;
wire net_101;
wire net_32;
wire net_111;
wire net_90;
wire net_40;
wire net_100;
wire n8gat;
wire net_85;
wire net_69;
wire net_124;
wire net_83;
wire net_115;
wire n63gat;
wire net_4;
wire net_95;
wire net_17;
wire net_78;
wire n17gat;
wire net_27;
wire n370gat;
wire net_56;
wire net_87;
wire net_0;
wire net_35;
wire net_22;
wire net_16;
wire net_39;
wire n92gat;
wire net_102;
wire net_2;
wire net_59;
wire n14gat;
wire net_9;
wire net_42;
wire net_120;
wire n95gat;
wire net_109;
wire net_80;
wire net_65;
wire net_50;
wire net_96;
wire net_66;
wire net_38;
wire net_44;
wire n421gat;
wire net_19;
wire n53gat;
wire net_89;
wire net_45;
wire n69gat;
wire net_126;
wire net_34;
wire n79gat;
wire net_108;
wire n50gat;
wire net_63;
wire n89gat;
wire n430gat;
wire n27gat;
wire net_116;
wire net_30;
wire net_91;
wire net_106;
wire net_99;
wire net_24;
wire net_55;
wire net_46;
wire net_118;
wire net_104;
wire n47gat;
wire net_72;
wire net_122;
wire net_25;
wire net_7;
wire net_70;
wire n4gat;
wire n431gat;
wire n76gat;
wire net_5;
wire net_52;
wire net_13;
wire net_94;
wire net_11;
wire net_18;
wire n73gat;
wire net_123;
wire n11gat;
wire net_114;
wire n60gat;
wire net_29;
wire net_68;
wire net_77;
wire net_20;
wire net_31;
wire net_36;
wire net_49;
wire net_15;
wire net_41;
wire net_57;
wire n21gat;
wire net_71;
wire net_3;
wire net_84;
wire net_112;
wire net_92;
wire net_1;
wire net_103;
wire net_43;
wire net_10;
wire net_28;
wire net_21;
wire net_51;
wire net_79;
wire n1gat;
wire net_97;
wire net_88;
wire net_60;
wire n115gat;
wire net_81;
wire net_58;
wire n86gat;
wire n329gat;
wire net_67;
wire net_82;
wire n223gat;
wire net_64;
wire net_37;
wire net_110;
wire net_121;
wire net_73;
wire net_33;
wire net_48;
wire net_86;
wire net_8;
wire net_75;
wire n108gat;
wire net_125;
wire n24gat;

// Start cells
INV_X1 inst_103 ( .ZN(net_3), .A(n79gat) );
INV_X1 inst_125 ( .A(net_113), .ZN(net_108) );
XNOR2_X1 inst_15 ( .ZN(net_81), .A(net_65), .B(n329gat) );
OR2_X4 inst_24 ( .A1(net_48), .ZN(net_26), .A2(n1gat) );
INV_X1 inst_114 ( .ZN(net_14), .A(n73gat) );
XNOR2_X1 inst_6 ( .ZN(net_45), .A(net_27), .B(n223gat) );
AND3_X4 inst_131 ( .ZN(net_55), .A3(net_44), .A2(net_6), .A1(n43gat) );
NAND2_X1 inst_76 ( .ZN(net_61), .A1(net_60), .A2(net_56) );
NOR3_X1 inst_33 ( .ZN(net_119), .A3(net_118), .A2(net_111), .A1(net_106) );
NAND2_X1 inst_83 ( .ZN(net_72), .A2(n329gat), .A1(n112gat) );
NAND4_X1 inst_47 ( .ZN(net_85), .A4(net_82), .A3(net_42), .A2(net_13), .A1(n95gat) );
OR3_X4 inst_19 ( .A2(net_30), .A3(net_29), .A1(net_28), .ZN(n223gat) );
INV_X1 inst_123 ( .A(net_116), .ZN(net_106) );
INV_X1 inst_121 ( .ZN(net_21), .A(net_20) );
XNOR2_X1 inst_2 ( .ZN(net_44), .A(net_24), .B(n223gat) );
XNOR2_X1 inst_8 ( .ZN(net_49), .A(net_19), .B(n223gat) );
INV_X1 inst_118 ( .ZN(net_16), .A(n115gat) );
NAND2_X1 inst_86 ( .ZN(net_95), .A2(n370gat), .A1(n115gat) );
OR3_X4 inst_20 ( .ZN(net_60), .A1(net_46), .A3(net_45), .A2(n21gat) );
NOR4_X1 inst_27 ( .ZN(net_64), .A1(net_63), .A3(net_62), .A2(net_61), .A4(net_52) );
NOR2_X1 inst_38 ( .ZN(net_29), .A1(net_11), .A2(n102gat) );
INV_X1 inst_100 ( .ZN(net_0), .A(n69gat) );
NAND4_X1 inst_52 ( .ZN(net_90), .A4(net_78), .A3(net_49), .A2(net_4), .A1(n30gat) );
NAND2_X1 inst_90 ( .ZN(net_99), .A2(n370gat), .A1(n14gat) );
NOR2_X1 inst_40 ( .A1(net_51), .ZN(net_20), .A2(n76gat) );
NAND2_X1 inst_93 ( .ZN(net_102), .A2(n370gat), .A1(n40gat) );
NAND2_X1 inst_81 ( .ZN(net_70), .A2(n329gat), .A1(n34gat) );
NAND2_X1 inst_95 ( .ZN(net_112), .A1(net_111), .A2(net_105) );
XNOR2_X1 inst_1 ( .ZN(net_42), .A(net_30), .B(n223gat) );
NAND2_X1 inst_72 ( .ZN(net_36), .A2(n223gat), .A1(n50gat) );
NAND4_X1 inst_59 ( .ZN(net_114), .A1(net_101), .A4(net_69), .A3(net_32), .A2(n43gat) );
NOR2_X1 inst_44 ( .ZN(net_123), .A2(net_114), .A1(net_108) );
NAND4_X1 inst_55 ( .ZN(net_116), .A1(net_98), .A4(net_66), .A3(net_33), .A2(n82gat) );
INV_X1 inst_115 ( .ZN(net_15), .A(n53gat) );
NOR2_X1 inst_37 ( .ZN(net_30), .A1(net_1), .A2(n89gat) );
XNOR2_X1 inst_5 ( .ZN(net_50), .A(net_21), .B(n223gat) );
NAND2_X1 inst_84 ( .ZN(net_73), .A2(n329gat), .A1(n99gat) );
NAND4_X1 inst_51 ( .ZN(net_89), .A4(net_77), .A3(net_41), .A2(net_9), .A1(n56gat) );
NAND2_X1 inst_80 ( .ZN(net_69), .A2(n329gat), .A1(n47gat) );
INV_X1 inst_105 ( .ZN(net_5), .A(n112gat) );
NAND2_X1 inst_68 ( .ZN(net_32), .A2(n223gat), .A1(n37gat) );
NAND2_X1 inst_78 ( .ZN(net_67), .A2(n329gat), .A1(n8gat) );
NOR2_X1 inst_42 ( .ZN(net_56), .A1(net_55), .A2(net_54) );
NAND4_X1 inst_53 ( .A4(net_94), .A1(net_87), .A2(net_86), .A3(net_85), .ZN(n370gat) );
AND2_X2 inst_133 ( .A1(net_125), .A2(net_104), .ZN(n421gat) );
NOR4_X1 inst_26 ( .ZN(net_25), .A1(net_24), .A2(net_23), .A3(net_22), .A4(net_20) );
INV_X1 inst_112 ( .ZN(net_12), .A(n60gat) );
NAND3_X1 inst_64 ( .ZN(net_93), .A1(net_90), .A2(net_89), .A3(net_88) );
INV_X1 inst_107 ( .ZN(net_7), .A(n43gat) );
NAND2_X1 inst_67 ( .ZN(net_31), .A2(n223gat), .A1(n1gat) );
AND3_X4 inst_127 ( .ZN(net_63), .A3(net_40), .A2(net_14), .A1(n69gat) );
NAND2_X1 inst_70 ( .ZN(net_34), .A2(n223gat), .A1(n11gat) );
AND3_X4 inst_129 ( .ZN(net_58), .A3(net_42), .A2(net_2), .A1(n95gat) );
NAND2_X1 inst_92 ( .ZN(net_101), .A2(n370gat), .A1(n53gat) );
NOR4_X1 inst_29 ( .ZN(net_91), .A4(net_79), .A1(net_48), .A3(net_47), .A2(n14gat) );
XNOR2_X1 inst_17 ( .ZN(net_83), .A(net_63), .B(n329gat) );
XNOR2_X1 inst_11 ( .ZN(net_77), .A(net_54), .B(n329gat) );
XNOR2_X1 inst_14 ( .ZN(net_80), .A(net_60), .B(n329gat) );
INV_X1 inst_122 ( .ZN(net_53), .A(net_52) );
NOR4_X1 inst_31 ( .ZN(net_94), .A2(net_93), .A3(net_92), .A4(net_91), .A1(net_84) );
OR2_X4 inst_25 ( .A1(net_46), .ZN(net_27), .A2(n11gat) );
INV_X1 inst_126 ( .ZN(net_110), .A(net_109) );
NAND4_X1 inst_62 ( .ZN(net_105), .A1(net_95), .A4(net_72), .A3(net_35), .A2(n108gat) );
INV_X1 inst_110 ( .ZN(net_10), .A(n24gat) );
NAND2_X1 inst_74 ( .ZN(net_38), .A2(n223gat), .A1(n89gat) );
NAND4_X1 inst_57 ( .ZN(net_107), .A1(net_100), .A4(net_68), .A3(net_36), .A2(n56gat) );
NOR3_X1 inst_35 ( .A2(net_124), .ZN(net_122), .A3(net_115), .A1(net_112) );
INV_X1 inst_99 ( .ZN(net_46), .A(n17gat) );
NAND4_X1 inst_48 ( .ZN(net_86), .A4(net_75), .A3(net_43), .A2(net_16), .A1(n108gat) );
NAND2_X1 inst_69 ( .ZN(net_33), .A2(n223gat), .A1(n76gat) );
NAND4_X1 inst_46 ( .ZN(net_28), .A1(net_27), .A3(net_26), .A4(net_25), .A2(net_18) );
NAND2_X1 inst_82 ( .ZN(net_71), .A2(n329gat), .A1(n21gat) );
NOR4_X1 inst_30 ( .ZN(net_92), .A4(net_80), .A1(net_46), .A3(net_45), .A2(n27gat) );
INV_X1 inst_102 ( .ZN(net_2), .A(n99gat) );
INV_X1 inst_108 ( .ZN(net_8), .A(n56gat) );
NOR3_X1 inst_32 ( .ZN(net_52), .A1(net_48), .A3(net_47), .A2(n8gat) );
OR3_X2 inst_22 ( .A2(net_124), .A3(net_123), .A1(net_120), .ZN(n430gat) );
NOR3_X1 inst_34 ( .ZN(net_126), .A1(net_121), .A2(net_120), .A3(net_118) );
XNOR2_X1 inst_12 ( .ZN(net_78), .A(net_62), .B(n329gat) );
NAND4_X1 inst_56 ( .ZN(net_104), .A1(net_99), .A4(net_67), .A3(net_31), .A2(n4gat) );
NAND2_X1 inst_71 ( .ZN(net_35), .A2(n223gat), .A1(n102gat) );
OR3_X4 inst_21 ( .ZN(net_65), .A1(net_51), .A3(net_50), .A2(n86gat) );
INV_X1 inst_104 ( .ZN(net_4), .A(n40gat) );
NAND4_X1 inst_60 ( .ZN(net_113), .A1(net_102), .A4(net_70), .A3(net_39), .A2(n30gat) );
NAND2_X1 inst_97 ( .ZN(net_115), .A1(net_114), .A2(net_107) );
INV_X1 inst_124 ( .ZN(net_120), .A(net_107) );
OR4_X1 inst_18 ( .A1(net_126), .A4(net_123), .A2(net_119), .A3(net_110), .ZN(n432gat) );
XNOR2_X1 inst_16 ( .ZN(net_82), .A(net_58), .B(n329gat) );
NAND2_X1 inst_88 ( .ZN(net_97), .A2(n370gat), .A1(n79gat) );
XNOR2_X1 inst_3 ( .ZN(net_41), .A(net_23), .B(n223gat) );
XNOR2_X1 inst_9 ( .ZN(net_75), .A(net_57), .B(n329gat) );
INV_X1 inst_113 ( .ZN(net_13), .A(n105gat) );
NAND4_X1 inst_50 ( .ZN(net_88), .A4(net_76), .A3(net_44), .A2(net_15), .A1(n43gat) );
NOR2_X1 inst_41 ( .ZN(net_24), .A1(net_7), .A2(n37gat) );
AND3_X4 inst_130 ( .ZN(net_57), .A3(net_43), .A2(net_5), .A1(n108gat) );
NAND2_X1 inst_91 ( .ZN(net_100), .A2(n370gat), .A1(n66gat) );
AND3_X4 inst_132 ( .ZN(net_62), .A3(net_49), .A2(net_17), .A1(n30gat) );
NAND4_X1 inst_58 ( .ZN(net_121), .A1(net_97), .A4(net_74), .A3(net_37), .A2(n69gat) );
NOR2_X1 inst_36 ( .ZN(net_22), .A1(net_0), .A2(n63gat) );
NAND2_X1 inst_87 ( .ZN(net_96), .A2(n370gat), .A1(n105gat) );
NAND4_X1 inst_61 ( .ZN(net_109), .A1(net_103), .A4(net_71), .A3(net_34), .A2(n17gat) );
NOR2_X1 inst_45 ( .ZN(net_117), .A1(net_116), .A2(net_115) );
NAND2_X1 inst_96 ( .ZN(net_118), .A2(net_114), .A1(net_113) );
INV_X1 inst_101 ( .ZN(net_1), .A(n95gat) );
XNOR2_X1 inst_0 ( .ZN(net_40), .A(net_22), .B(n223gat) );
XNOR2_X1 inst_10 ( .ZN(net_76), .A(net_55), .B(n329gat) );
XNOR2_X1 inst_4 ( .ZN(net_43), .A(net_29), .B(n223gat) );
NAND3_X1 inst_65 ( .ZN(net_125), .A3(net_122), .A2(net_121), .A1(net_116) );
NAND2_X1 inst_89 ( .ZN(net_98), .A2(n370gat), .A1(n92gat) );
NOR4_X1 inst_28 ( .ZN(net_84), .A4(net_81), .A1(net_51), .A3(net_50), .A2(n92gat) );
INV_X1 inst_111 ( .ZN(net_11), .A(n108gat) );
NAND2_X1 inst_66 ( .ZN(net_18), .A2(net_10), .A1(n30gat) );
INV_X1 inst_117 ( .ZN(net_48), .A(n4gat) );
NAND2_X1 inst_98 ( .ZN(net_124), .A2(net_113), .A1(net_109) );
NAND3_X2 inst_63 ( .A2(net_65), .A1(net_64), .A3(net_59), .ZN(n329gat) );
XNOR2_X1 inst_7 ( .ZN(net_47), .A(net_26), .B(n223gat) );
NAND4_X1 inst_49 ( .ZN(net_87), .A4(net_83), .A3(net_40), .A2(net_3), .A1(n69gat) );
INV_X1 inst_120 ( .ZN(net_19), .A(net_18) );
XNOR2_X1 inst_13 ( .ZN(net_79), .A(net_53), .B(n329gat) );
INV_X1 inst_119 ( .ZN(net_17), .A(n34gat) );
NAND2_X1 inst_75 ( .ZN(net_39), .A2(n223gat), .A1(n24gat) );
INV_X1 inst_116 ( .ZN(net_51), .A(n82gat) );
NAND2_X1 inst_85 ( .ZN(net_74), .A2(n329gat), .A1(n73gat) );
NAND4_X1 inst_54 ( .ZN(net_111), .A1(net_96), .A4(net_73), .A3(net_38), .A2(n95gat) );
NAND2_X1 inst_79 ( .ZN(net_68), .A2(n329gat), .A1(n60gat) );
INV_X1 inst_109 ( .ZN(net_9), .A(n66gat) );
INV_X1 inst_106 ( .ZN(net_6), .A(n47gat) );
NOR2_X1 inst_43 ( .ZN(net_59), .A1(net_58), .A2(net_57) );
NOR2_X1 inst_39 ( .ZN(net_23), .A1(net_8), .A2(n50gat) );
AND3_X4 inst_128 ( .ZN(net_54), .A3(net_41), .A2(net_12), .A1(n56gat) );
NAND2_X1 inst_73 ( .ZN(net_37), .A2(n223gat), .A1(n63gat) );
OR3_X2 inst_23 ( .A3(net_126), .A2(net_124), .A1(net_117), .ZN(n431gat) );
NAND2_X1 inst_77 ( .ZN(net_66), .A2(n329gat), .A1(n86gat) );
NAND2_X1 inst_94 ( .ZN(net_103), .A2(n370gat), .A1(n27gat) );

endmodule
