module c499 (
nid20,
nid6,
nid21,
nic7,
nid0,
nid11,
nid13,
nr,
nic0,
nid1,
nic4,
nid10,
nid31,
nid3,
nic1,
nid22,
nic3,
nid26,
nid23,
nid16,
nid17,
nid27,
nid9,
nic2,
nid7,
nic5,
nid30,
nic6,
nid14,
nid29,
nid8,
nid12,
nid5,
nid15,
nid25,
nid18,
nid28,
nid24,
nid4,
nid19,
nid2,
nod24,
nod6,
nod23,
nod31,
nod14,
nod11,
nod19,
nod28,
nod12,
nod5,
nod13,
nod3,
nod21,
nod29,
nod1,
nod7,
nod17,
nod30,
nod18,
nod4,
nod10,
nod15,
nod0,
nod16,
nod20,
nod27,
nod2,
nod9,
nod25,
nod8,
nod26,
nod22);

// Start PIs
input nid20;
input nid6;
input nid21;
input nic7;
input nid0;
input nid11;
input nid13;
input nr;
input nic0;
input nid1;
input nic4;
input nid10;
input nid31;
input nid3;
input nic1;
input nid22;
input nic3;
input nid26;
input nid23;
input nid16;
input nid17;
input nid27;
input nid9;
input nic2;
input nid7;
input nic5;
input nid30;
input nic6;
input nid14;
input nid29;
input nid8;
input nid12;
input nid5;
input nid15;
input nid25;
input nid18;
input nid28;
input nid24;
input nid4;
input nid19;
input nid2;

// Start POs
output nod24;
output nod6;
output nod23;
output nod31;
output nod14;
output nod11;
output nod19;
output nod28;
output nod12;
output nod5;
output nod13;
output nod3;
output nod21;
output nod29;
output nod1;
output nod7;
output nod17;
output nod30;
output nod18;
output nod4;
output nod10;
output nod15;
output nod0;
output nod16;
output nod20;
output nod27;
output nod2;
output nod9;
output nod25;
output nod8;
output nod26;
output nod22;

// Start wires
wire net_107;
wire net_47;
wire nod23;
wire net_61;
wire net_137;
wire nid21;
wire net_132;
wire net_54;
wire net_105;
wire net_62;
wire net_6;
wire nid13;
wire net_129;
wire net_119;
wire net_98;
wire nod30;
wire net_23;
wire net_117;
wire net_12;
wire net_74;
wire net_53;
wire net_93;
wire net_135;
wire nid3;
wire net_130;
wire nic3;
wire nod26;
wire nid26;
wire nid23;
wire net_127;
wire net_14;
wire nid9;
wire net_113;
wire net_26;
wire net_76;
wire nid7;
wire net_101;
wire net_32;
wire nic6;
wire net_111;
wire net_90;
wire net_40;
wire nod7;
wire net_100;
wire nod4;
wire net_85;
wire net_69;
wire net_124;
wire nid5;
wire nid15;
wire nid25;
wire nid18;
wire net_141;
wire nid28;
wire net_83;
wire net_115;
wire net_4;
wire nid2;
wire net_95;
wire net_17;
wire net_78;
wire net_27;
wire nid20;
wire net_56;
wire net_87;
wire nod19;
wire net_0;
wire nod28;
wire nid0;
wire net_35;
wire net_22;
wire net_16;
wire nic0;
wire nod17;
wire net_39;
wire net_102;
wire net_2;
wire net_59;
wire net_9;
wire net_42;
wire net_120;
wire nic1;
wire net_109;
wire net_80;
wire net_65;
wire nod25;
wire net_50;
wire nod8;
wire net_96;
wire net_66;
wire net_38;
wire net_44;
wire nod11;
wire net_136;
wire net_134;
wire nod13;
wire net_19;
wire nid29;
wire net_89;
wire net_45;
wire net_126;
wire nod1;
wire net_34;
wire net_108;
wire nod15;
wire net_63;
wire nid6;
wire net_116;
wire net_30;
wire net_91;
wire nod21;
wire nod3;
wire net_106;
wire net_99;
wire net_24;
wire net_55;
wire net_46;
wire net_140;
wire net_118;
wire nr;
wire net_104;
wire nic4;
wire nid10;
wire nod16;
wire net_72;
wire net_122;
wire net_25;
wire net_70;
wire net_7;
wire nod9;
wire nod22;
wire nid17;
wire nid27;
wire nod6;
wire net_5;
wire net_52;
wire net_128;
wire net_138;
wire nic5;
wire nod12;
wire net_13;
wire nid30;
wire net_94;
wire nid12;
wire nod18;
wire net_11;
wire net_18;
wire net_123;
wire net_131;
wire net_114;
wire net_29;
wire net_68;
wire nod27;
wire net_142;
wire net_77;
wire net_20;
wire net_31;
wire net_36;
wire net_49;
wire net_15;
wire net_57;
wire nod31;
wire net_41;
wire net_71;
wire nic7;
wire nod5;
wire nid11;
wire net_3;
wire net_84;
wire nod29;
wire net_112;
wire net_92;
wire net_1;
wire net_103;
wire nid1;
wire net_139;
wire nod10;
wire net_43;
wire net_10;
wire nid31;
wire net_28;
wire net_21;
wire net_51;
wire net_79;
wire nid22;
wire net_143;
wire net_97;
wire nod24;
wire nid16;
wire net_88;
wire net_60;
wire nod14;
wire nic2;
wire net_81;
wire net_58;
wire nid14;
wire nid8;
wire net_82;
wire net_67;
wire net_64;
wire net_37;
wire net_110;
wire net_121;
wire net_73;
wire nod0;
wire nod20;
wire net_33;
wire net_48;
wire net_86;
wire net_75;
wire net_8;
wire nid24;
wire nid4;
wire net_133;
wire nod2;
wire nid19;
wire net_125;

// Start cells
NAND2_X1 inst_145 ( .A1(net_139), .A2(net_135), .ZN(net_104) );
XNOR2_X1 inst_103 ( .B(net_121), .ZN(nod10), .A(nid10) );
NAND2_X1 inst_125 ( .ZN(net_2), .A2(nr), .A1(nic7) );
NAND2_X1 inst_138 ( .A1(net_120), .A2(net_100), .ZN(net_96) );
NAND2_X1 inst_159 ( .A2(net_133), .ZN(net_127), .A1(net_110) );
XOR2_X1 inst_15 ( .Z(net_29), .A(nid20), .B(nid16) );
NAND2_X1 inst_134 ( .A2(net_95), .A1(net_93), .ZN(net_77) );
XOR2_X1 inst_24 ( .Z(net_38), .A(nid23), .B(nid19) );
NOR2_X1 inst_114 ( .ZN(net_116), .A2(net_94), .A1(net_93) );
XOR2_X1 inst_6 ( .Z(net_15), .A(nid6), .B(nid2) );
NAND2_X1 inst_131 ( .A1(net_117), .ZN(net_90), .A2(net_72) );
XNOR2_X1 inst_76 ( .B(net_126), .ZN(nod7), .A(nid7) );
NAND2_X1 inst_160 ( .A2(net_135), .ZN(net_129), .A1(net_128) );
NAND2_X1 inst_150 ( .A2(net_116), .ZN(net_111), .A1(net_110) );
XNOR2_X1 inst_33 ( .ZN(net_40), .B(net_27), .A(net_17) );
INV_X1 inst_172 ( .A(net_110), .ZN(net_89) );
XNOR2_X1 inst_83 ( .B(net_106), .ZN(nod29), .A(nid29) );
XNOR2_X1 inst_47 ( .ZN(net_47), .B(net_38), .A(net_37) );
XOR2_X1 inst_19 ( .Z(net_33), .A(nid30), .B(nid26) );
NAND2_X1 inst_123 ( .ZN(net_0), .A2(nr), .A1(nic0) );
NAND3_X1 inst_121 ( .A2(net_120), .ZN(net_94), .A3(net_86), .A1(net_83) );
XOR2_X1 inst_2 ( .Z(net_11), .A(nid8), .B(nid12) );
XOR2_X1 inst_8 ( .Z(net_17), .A(nid7), .B(nid3) );
NAND3_X1 inst_118 ( .A1(net_89), .ZN(net_81), .A2(net_80), .A3(net_76) );
XNOR2_X1 inst_86 ( .B(net_113), .ZN(nod26), .A(nid26) );
NAND2_X1 inst_153 ( .ZN(net_118), .A1(net_117), .A2(net_116) );
XOR2_X1 inst_20 ( .Z(net_34), .A(nid21), .B(nid20) );
XNOR2_X1 inst_27 ( .ZN(net_18), .A(nid17), .B(nid16) );
XNOR2_X1 inst_38 ( .ZN(net_58), .B(net_39), .A(net_20) );
XNOR2_X1 inst_100 ( .B(net_109), .ZN(nod13), .A(nid13) );
XNOR2_X1 inst_52 ( .ZN(net_59), .A(net_58), .B(net_51) );
XNOR2_X1 inst_90 ( .B(net_130), .ZN(nod22), .A(nid22) );
NAND2_X1 inst_140 ( .A2(net_114), .A1(net_110), .ZN(net_98) );
XNOR2_X1 inst_40 ( .ZN(net_49), .A(net_34), .B(net_9) );
NAND2_X1 inst_162 ( .A2(net_138), .ZN(net_132), .A1(net_131) );
NAND2_X1 inst_167 ( .ZN(net_141), .A1(net_139), .A2(net_124) );
XNOR2_X1 inst_93 ( .B(net_142), .ZN(nod2), .A(nid2) );
XNOR2_X1 inst_81 ( .B(net_102), .ZN(nod30), .A(nid30) );
XNOR2_X1 inst_95 ( .B(net_107), .ZN(nod18), .A(nid18) );
XOR2_X1 inst_1 ( .Z(net_10), .A(nid31), .B(nid30) );
XNOR2_X1 inst_72 ( .B(net_139), .A(net_120), .ZN(net_74) );
NAND2_X1 inst_139 ( .A1(net_128), .A2(net_100), .ZN(net_97) );
NAND2_X1 inst_155 ( .A2(net_138), .ZN(net_121), .A1(net_120) );
XNOR2_X1 inst_59 ( .ZN(net_67), .A(net_59), .B(net_1) );
NAND2_X1 inst_135 ( .ZN(net_85), .A2(net_84), .A1(net_78) );
XNOR2_X1 inst_44 ( .ZN(net_45), .A(net_33), .B(net_23) );
XNOR2_X1 inst_55 ( .ZN(net_63), .A(net_56), .B(net_54) );
INV_X1 inst_174 ( .ZN(net_139), .A(net_83) );
NOR2_X1 inst_115 ( .ZN(net_133), .A1(net_95), .A2(net_92) );
XNOR2_X1 inst_37 ( .ZN(net_54), .A(net_31), .B(net_18) );
NAND2_X1 inst_148 ( .A1(net_122), .ZN(net_108), .A2(net_105) );
NAND2_X1 inst_164 ( .ZN(net_136), .A2(net_135), .A1(net_131) );
XOR2_X1 inst_5 ( .Z(net_14), .A(nid9), .B(nid8) );
NAND2_X1 inst_157 ( .A1(net_128), .ZN(net_125), .A2(net_124) );
XNOR2_X1 inst_84 ( .B(net_108), .ZN(nod28), .A(nid28) );
XNOR2_X1 inst_51 ( .ZN(net_57), .A(net_56), .B(net_48) );
NAND2_X1 inst_142 ( .A1(net_131), .ZN(net_101), .A2(net_100) );
XNOR2_X1 inst_80 ( .B(net_99), .ZN(nod31), .A(nid31) );
INV_X1 inst_173 ( .A(net_120), .ZN(net_82) );
XNOR2_X1 inst_105 ( .B(net_129), .ZN(nod0), .A(nid0) );
XNOR2_X1 inst_68 ( .ZN(net_120), .B(net_70), .A(net_44) );
XNOR2_X1 inst_78 ( .B(net_101), .ZN(nod5), .A(nid5) );
XNOR2_X1 inst_42 ( .ZN(net_60), .A(net_35), .B(net_24) );
INV_X1 inst_175 ( .A(net_122), .ZN(net_72) );
XNOR2_X1 inst_53 ( .ZN(net_61), .B(net_60), .A(net_52) );
NAND2_X1 inst_133 ( .A2(net_90), .A1(net_88), .ZN(net_76) );
XNOR2_X1 inst_26 ( .ZN(net_8), .A(nid11), .B(nid10) );
NAND2_X1 inst_151 ( .A2(net_116), .ZN(net_113), .A1(net_112) );
NOR2_X1 inst_112 ( .ZN(net_135), .A2(net_91), .A1(net_88) );
XNOR2_X1 inst_64 ( .ZN(net_80), .B(net_67), .A(net_45) );
OR3_X4 inst_107 ( .A2(net_122), .A1(net_117), .ZN(net_79), .A3(net_75) );
XNOR2_X1 inst_67 ( .ZN(net_117), .B(net_65), .A(net_43) );
NAND2_X1 inst_127 ( .ZN(net_4), .A2(nr), .A1(nic5) );
XNOR2_X1 inst_70 ( .ZN(net_122), .B(net_71), .A(net_41) );
NAND2_X1 inst_129 ( .ZN(net_6), .A2(nr), .A1(nic1) );
XNOR2_X1 inst_92 ( .B(net_137), .ZN(nod20), .A(nid20) );
XNOR2_X1 inst_29 ( .ZN(net_24), .A(nid7), .B(nid6) );
XOR2_X1 inst_17 ( .Z(net_31), .A(nid19), .B(nid18) );
XOR2_X1 inst_11 ( .Z(net_21), .A(nid9), .B(nid13) );
NAND2_X1 inst_146 ( .A1(net_117), .ZN(net_106), .A2(net_105) );
XOR2_X1 inst_14 ( .Z(net_28), .A(nid13), .B(nid12) );
NAND3_X1 inst_122 ( .A2(net_112), .ZN(net_91), .A1(net_89), .A3(net_85) );
XNOR2_X1 inst_31 ( .ZN(net_27), .A(nid15), .B(nid11) );
XOR2_X1 inst_25 ( .Z(net_39), .A(nid3), .B(nid2) );
NAND2_X1 inst_126 ( .ZN(net_3), .A2(nr), .A1(nic3) );
NAND2_X1 inst_158 ( .A1(net_139), .ZN(net_126), .A2(net_100) );
NAND2_X1 inst_141 ( .A1(net_110), .A2(net_105), .ZN(net_99) );
XNOR2_X1 inst_62 ( .ZN(net_70), .A(net_63), .B(net_7) );
NOR3_X1 inst_110 ( .ZN(net_100), .A1(net_89), .A2(net_88), .A3(net_87) );
XNOR2_X1 inst_74 ( .B(net_132), .ZN(nod9), .A(nid9) );
XNOR2_X1 inst_57 ( .ZN(net_65), .A(net_53), .B(net_4) );
XNOR2_X1 inst_35 ( .ZN(net_42), .A(net_30), .B(net_11) );
XNOR2_X1 inst_99 ( .B(net_103), .ZN(nod14), .A(nid14) );
XNOR2_X1 inst_48 ( .ZN(net_50), .A(net_49), .B(net_48) );
XNOR2_X1 inst_69 ( .ZN(net_83), .B(net_64), .A(net_40) );
XNOR2_X1 inst_46 ( .ZN(net_51), .A(net_14), .B(net_8) );
XNOR2_X1 inst_82 ( .B(net_104), .ZN(nod3), .A(nid3) );
NAND2_X1 inst_136 ( .ZN(net_86), .A2(net_81), .A1(net_79) );
XNOR2_X1 inst_30 ( .ZN(net_26), .A(nid29), .B(nid28) );
XNOR2_X1 inst_102 ( .B(net_140), .ZN(nod11), .A(nid11) );
OR2_X4 inst_108 ( .A2(net_117), .ZN(net_88), .A1(net_72) );
NAND2_X1 inst_165 ( .ZN(net_137), .A2(net_133), .A1(net_122) );
XNOR2_X1 inst_32 ( .ZN(net_52), .B(net_28), .A(net_12) );
XOR2_X1 inst_22 ( .Z(net_36), .A(nid5), .B(nid1) );
NAND2_X1 inst_144 ( .A2(net_124), .A1(net_120), .ZN(net_103) );
XNOR2_X1 inst_34 ( .ZN(net_41), .B(net_29), .A(net_19) );
XOR2_X1 inst_12 ( .Z(net_22), .A(nid29), .B(nid25) );
XNOR2_X1 inst_56 ( .ZN(net_64), .A(net_50), .B(net_3) );
XNOR2_X1 inst_71 ( .ZN(net_131), .B(net_66), .A(net_46) );
XOR2_X1 inst_21 ( .Z(net_35), .A(nid5), .B(nid4) );
XNOR2_X1 inst_104 ( .B(net_136), .ZN(nod1), .A(nid1) );
XNOR2_X1 inst_60 ( .ZN(net_68), .A(net_61), .B(net_2) );
NAND2_X1 inst_169 ( .ZN(net_143), .A2(net_138), .A1(net_128) );
NAND2_X1 inst_168 ( .ZN(net_142), .A2(net_135), .A1(net_120) );
XNOR2_X1 inst_97 ( .B(net_119), .ZN(nod16), .A(nid16) );
NAND2_X1 inst_161 ( .A2(net_133), .ZN(net_130), .A1(net_112) );
NAND2_X1 inst_124 ( .ZN(net_1), .A2(nr), .A1(nic6) );
XOR2_X1 inst_18 ( .Z(net_32), .A(nid25), .B(nid24) );
XOR2_X1 inst_16 ( .Z(net_30), .A(nid4), .B(nid0) );
XNOR2_X1 inst_88 ( .B(net_123), .ZN(nod24), .A(nid24) );
XOR2_X1 inst_3 ( .Z(net_12), .A(nid15), .B(nid14) );
NAND2_X1 inst_156 ( .ZN(net_123), .A1(net_122), .A2(net_116) );
XOR2_X1 inst_9 ( .Z(net_19), .A(nid28), .B(nid24) );
NOR2_X1 inst_113 ( .ZN(net_105), .A1(net_93), .A2(net_92) );
INV_X1 inst_170 ( .ZN(net_112), .A(net_80) );
XNOR2_X1 inst_50 ( .ZN(net_55), .B(net_54), .A(net_49) );
NAND2_X1 inst_137 ( .ZN(net_87), .A2(net_85), .A1(net_80) );
XNOR2_X1 inst_41 ( .ZN(net_56), .A(net_32), .B(net_25) );
NAND2_X1 inst_130 ( .ZN(net_7), .A2(nr), .A1(nic2) );
XNOR2_X1 inst_91 ( .B(net_134), .ZN(nod21), .A(nid21) );
NAND2_X1 inst_132 ( .A1(net_131), .ZN(net_93), .A2(net_73) );
NAND2_X1 inst_143 ( .A1(net_112), .A2(net_105), .ZN(net_102) );
NAND2_X1 inst_152 ( .A1(net_117), .ZN(net_115), .A2(net_114) );
XNOR2_X1 inst_58 ( .ZN(net_66), .A(net_57), .B(net_6) );
XNOR2_X1 inst_36 ( .ZN(net_48), .B(net_26), .A(net_10) );
NAND2_X1 inst_147 ( .A2(net_114), .A1(net_112), .ZN(net_107) );
XNOR2_X1 inst_87 ( .B(net_118), .ZN(nod25), .A(nid25) );
XNOR2_X1 inst_61 ( .ZN(net_69), .A(net_55), .B(net_0) );
XNOR2_X1 inst_45 ( .ZN(net_46), .A(net_36), .B(net_21) );
XNOR2_X1 inst_96 ( .B(net_115), .ZN(nod17), .A(nid17) );
XNOR2_X1 inst_101 ( .B(net_125), .ZN(nod12), .A(nid12) );
XOR2_X1 inst_0 ( .Z(net_9), .A(nid23), .B(nid22) );
XOR2_X1 inst_10 ( .Z(net_20), .A(nid1), .B(nid0) );
XOR2_X1 inst_4 ( .Z(net_13), .A(nid21), .B(nid17) );
XNOR2_X1 inst_65 ( .ZN(net_128), .B(net_69), .A(net_42) );
XNOR2_X1 inst_89 ( .B(net_127), .ZN(nod23), .A(nid23) );
XNOR2_X1 inst_28 ( .ZN(net_23), .A(nid22), .B(nid18) );
NOR3_X1 inst_111 ( .ZN(net_124), .A2(net_90), .A1(net_89), .A3(net_87) );
XNOR2_X1 inst_66 ( .ZN(net_110), .B(net_68), .A(net_47) );
NOR2_X1 inst_117 ( .ZN(net_138), .A2(net_91), .A1(net_90) );
XNOR2_X1 inst_98 ( .B(net_141), .ZN(nod15), .A(nid15) );
XNOR2_X1 inst_63 ( .ZN(net_71), .A(net_62), .B(net_5) );
XOR2_X1 inst_7 ( .Z(net_16), .A(nid14), .B(nid10) );
XNOR2_X1 inst_49 ( .ZN(net_53), .A(net_52), .B(net_51) );
NAND3_X1 inst_120 ( .A1(net_139), .ZN(net_92), .A3(net_86), .A2(net_82) );
NAND2_X1 inst_154 ( .A1(net_122), .ZN(net_119), .A2(net_114) );
XOR2_X1 inst_13 ( .Z(net_25), .A(nid27), .B(nid26) );
NAND3_X1 inst_119 ( .ZN(net_84), .A1(net_83), .A2(net_82), .A3(net_77) );
XNOR2_X1 inst_75 ( .B(net_143), .ZN(nod8), .A(nid8) );
NAND2_X1 inst_166 ( .ZN(net_140), .A1(net_139), .A2(net_138) );
NOR2_X1 inst_116 ( .ZN(net_114), .A1(net_95), .A2(net_94) );
NAND2_X1 inst_163 ( .ZN(net_134), .A2(net_133), .A1(net_117) );
XNOR2_X1 inst_85 ( .B(net_111), .ZN(nod27), .A(nid27) );
XNOR2_X1 inst_54 ( .ZN(net_62), .B(net_60), .A(net_58) );
XNOR2_X1 inst_79 ( .B(net_97), .ZN(nod4), .A(nid4) );
OR2_X4 inst_109 ( .A2(net_131), .ZN(net_95), .A1(net_73) );
OR3_X4 inst_106 ( .A1(net_131), .A2(net_128), .ZN(net_78), .A3(net_74) );
NAND2_X1 inst_149 ( .A1(net_131), .A2(net_124), .ZN(net_109) );
XNOR2_X1 inst_43 ( .ZN(net_44), .B(net_16), .A(net_15) );
XNOR2_X1 inst_39 ( .ZN(net_43), .A(net_22), .B(net_13) );
NAND2_X1 inst_128 ( .ZN(net_5), .A2(nr), .A1(nic4) );
XNOR2_X1 inst_73 ( .B(net_112), .A(net_110), .ZN(net_75) );
XOR2_X1 inst_23 ( .Z(net_37), .A(nid31), .B(nid27) );
INV_X1 inst_171 ( .A(net_128), .ZN(net_73) );
XNOR2_X1 inst_77 ( .B(net_96), .ZN(nod6), .A(nid6) );
XNOR2_X1 inst_94 ( .B(net_98), .ZN(nod19), .A(nid19) );

endmodule
