module c1908 (
n227,
n902,
n217,
n237,
n143,
n131,
n110,
n134,
n952,
n221,
n900,
n140,
n113,
n234,
n146,
n122,
n472,
n104,
n107,
n128,
n953,
n101,
n125,
n224,
n116,
n210,
n475,
n119,
n478,
n898,
n214,
n137,
n469,
n66,
n72,
n69,
n54,
n18,
n24,
n75,
n60,
n39,
n45,
n42,
n30,
n63,
n57,
n9,
n21,
n51,
n33,
n6,
n15,
n12,
n3,
n27,
n48,
n36);

// Start PIs
input n227;
input n902;
input n217;
input n237;
input n143;
input n131;
input n110;
input n134;
input n952;
input n221;
input n900;
input n140;
input n113;
input n234;
input n146;
input n122;
input n472;
input n104;
input n107;
input n128;
input n953;
input n101;
input n125;
input n224;
input n116;
input n210;
input n475;
input n119;
input n478;
input n898;
input n214;
input n137;
input n469;

// Start POs
output n66;
output n72;
output n69;
output n54;
output n18;
output n24;
output n75;
output n60;
output n39;
output n45;
output n42;
output n30;
output n63;
output n57;
output n9;
output n21;
output n51;
output n33;
output n6;
output n15;
output n12;
output n3;
output n27;
output n48;
output n36;

// Start wires
wire net_166;
wire net_107;
wire net_47;
wire net_179;
wire net_176;
wire net_159;
wire net_61;
wire net_137;
wire net_132;
wire net_54;
wire n18;
wire net_105;
wire net_62;
wire net_6;
wire net_129;
wire net_119;
wire net_98;
wire net_23;
wire net_117;
wire net_12;
wire net_151;
wire net_74;
wire net_53;
wire net_93;
wire n63;
wire net_168;
wire net_135;
wire n27;
wire net_130;
wire n113;
wire net_147;
wire net_127;
wire net_14;
wire n146;
wire net_113;
wire net_26;
wire net_76;
wire n128;
wire n101;
wire net_101;
wire net_32;
wire net_187;
wire net_111;
wire net_90;
wire net_40;
wire n116;
wire net_100;
wire n210;
wire n475;
wire net_85;
wire net_69;
wire net_124;
wire net_161;
wire n119;
wire net_141;
wire net_160;
wire net_83;
wire n21;
wire net_115;
wire net_4;
wire n36;
wire net_95;
wire net_17;
wire net_173;
wire net_78;
wire net_27;
wire net_164;
wire n69;
wire net_56;
wire net_87;
wire net_0;
wire net_155;
wire net_35;
wire net_191;
wire n143;
wire net_22;
wire net_16;
wire net_181;
wire n131;
wire net_193;
wire net_39;
wire net_157;
wire net_144;
wire net_102;
wire net_2;
wire net_59;
wire net_9;
wire n952;
wire n57;
wire net_42;
wire n9;
wire net_120;
wire net_109;
wire n12;
wire net_80;
wire net_65;
wire net_50;
wire net_162;
wire n234;
wire net_96;
wire net_66;
wire net_38;
wire net_44;
wire net_167;
wire n122;
wire net_136;
wire net_134;
wire n107;
wire net_19;
wire net_89;
wire net_45;
wire net_126;
wire n45;
wire net_185;
wire net_34;
wire net_108;
wire n478;
wire net_183;
wire net_178;
wire net_150;
wire net_63;
wire n137;
wire n227;
wire n902;
wire n72;
wire net_152;
wire net_116;
wire net_30;
wire net_189;
wire n54;
wire net_175;
wire net_91;
wire n237;
wire n75;
wire net_106;
wire net_99;
wire net_24;
wire net_55;
wire net_186;
wire n39;
wire n60;
wire net_46;
wire net_140;
wire net_118;
wire net_148;
wire net_104;
wire n42;
wire net_146;
wire n134;
wire net_72;
wire net_122;
wire n221;
wire net_25;
wire net_70;
wire n900;
wire n51;
wire net_7;
wire n140;
wire n48;
wire net_194;
wire net_172;
wire net_5;
wire net_52;
wire net_165;
wire net_128;
wire n472;
wire n104;
wire net_138;
wire net_13;
wire net_184;
wire net_94;
wire net_11;
wire net_18;
wire net_123;
wire net_131;
wire net_114;
wire n30;
wire net_196;
wire net_170;
wire net_29;
wire net_68;
wire net_149;
wire net_142;
wire net_77;
wire n898;
wire n214;
wire net_20;
wire net_31;
wire n3;
wire n469;
wire net_36;
wire net_49;
wire net_158;
wire net_15;
wire net_41;
wire net_57;
wire n217;
wire net_71;
wire net_153;
wire net_156;
wire net_3;
wire net_84;
wire net_174;
wire net_154;
wire net_112;
wire net_92;
wire net_1;
wire net_103;
wire net_139;
wire n110;
wire net_43;
wire net_10;
wire net_180;
wire net_28;
wire net_169;
wire net_21;
wire net_51;
wire net_171;
wire net_79;
wire n15;
wire net_143;
wire net_97;
wire net_190;
wire n66;
wire net_88;
wire net_182;
wire net_192;
wire net_145;
wire net_60;
wire n953;
wire net_81;
wire n24;
wire net_163;
wire net_58;
wire n125;
wire net_82;
wire net_67;
wire n224;
wire net_64;
wire net_37;
wire net_188;
wire net_110;
wire net_121;
wire net_73;
wire net_33;
wire net_48;
wire net_177;
wire net_86;
wire net_75;
wire net_8;
wire net_133;
wire n6;
wire n33;
wire net_195;
wire net_125;

// Start cells
NAND3_X1 inst_145 ( .A3(net_142), .A2(net_121), .ZN(net_120), .A1(net_119) );
NOR2_X1 inst_103 ( .ZN(net_60), .A1(net_59), .A2(net_58) );
NOR2_X1 inst_125 ( .ZN(net_162), .A2(net_153), .A1(net_152) );
AND4_X1 inst_207 ( .A1(net_121), .ZN(net_115), .A3(net_114), .A4(net_113), .A2(net_94) );
NAND4_X1 inst_138 ( .ZN(net_149), .A2(net_121), .A3(net_114), .A1(net_111), .A4(net_100) );
NAND2_X1 inst_159 ( .ZN(net_101), .A2(net_21), .A1(n214) );
AND2_X4 inst_218 ( .A1(net_114), .ZN(net_107), .A2(net_94) );
XOR2_X1 inst_15 ( .Z(net_82), .A(net_75), .B(n469) );
INV_X1 inst_197 ( .ZN(net_143), .A(net_88) );
NOR2_X1 inst_134 ( .A2(net_195), .A1(net_194), .ZN(n57) );
NAND2_X1 inst_179 ( .ZN(net_157), .A2(net_144), .A1(net_143) );
XOR2_X1 inst_24 ( .A(net_156), .Z(n12), .B(n110) );
NOR2_X1 inst_114 ( .A2(net_130), .ZN(net_118), .A1(net_88) );
XOR2_X1 inst_6 ( .Z(net_13), .A(n137), .B(n134) );
INV_X1 inst_194 ( .ZN(net_85), .A(net_80) );
NOR2_X1 inst_131 ( .A1(net_194), .A2(net_187), .ZN(n54) );
OR2_X4 inst_76 ( .A1(net_121), .ZN(net_110), .A2(net_31) );
AND2_X4 inst_214 ( .A1(net_59), .A2(net_58), .ZN(net_57) );
NAND2_X1 inst_180 ( .ZN(net_160), .A2(net_144), .A1(net_142) );
NAND2_X1 inst_160 ( .ZN(net_22), .A2(net_2), .A1(n953) );
NAND3_X1 inst_150 ( .ZN(net_181), .A3(net_180), .A1(n902), .A2(n472) );
XNOR2_X1 inst_33 ( .ZN(net_36), .A(net_7), .B(n110) );
NAND2_X1 inst_172 ( .ZN(net_126), .A1(net_108), .A2(net_107) );
NOR4_X1 inst_83 ( .ZN(net_183), .A4(net_175), .A2(net_170), .A1(net_115), .A3(net_112) );
XNOR2_X1 inst_47 ( .ZN(net_65), .B(net_43), .A(net_10) );
XOR2_X1 inst_19 ( .A(net_145), .Z(n18), .B(n116) );
NOR2_X1 inst_123 ( .ZN(net_144), .A2(net_128), .A1(net_89) );
NOR2_X1 inst_121 ( .ZN(net_146), .A1(net_126), .A2(net_103) );
XOR2_X1 inst_2 ( .Z(net_7), .A(n128), .B(n119) );
XOR2_X1 inst_8 ( .Z(net_16), .A(n143), .B(n128) );
NOR2_X1 inst_118 ( .ZN(net_111), .A1(net_101), .A2(net_99) );
NOR3_X1 inst_86 ( .ZN(net_56), .A3(net_52), .A1(net_51), .A2(net_11) );
NAND2_X1 inst_153 ( .ZN(net_3), .A2(n898), .A1(n224) );
XOR2_X1 inst_20 ( .A(net_146), .Z(n15), .B(n113) );
XOR2_X1 inst_27 ( .Z(net_176), .A(net_171), .B(net_56) );
XNOR2_X1 inst_38 ( .ZN(net_59), .A(net_26), .B(n101) );
NOR2_X1 inst_100 ( .ZN(net_31), .A2(net_30), .A1(n900) );
XNOR2_X1 inst_52 ( .ZN(net_71), .A(net_69), .B(net_65) );
NOR3_X1 inst_90 ( .ZN(net_155), .A1(net_134), .A2(net_133), .A3(net_132) );
NAND4_X1 inst_140 ( .ZN(net_175), .A4(net_174), .A2(net_172), .A3(net_137), .A1(n952) );
AND4_X1 inst_209 ( .ZN(net_196), .A1(net_191), .A2(net_131), .A3(net_125), .A4(net_122) );
AND3_X4 inst_211 ( .ZN(net_129), .A2(net_110), .A1(net_106), .A3(net_105) );
AND2_X4 inst_221 ( .ZN(net_159), .A1(net_143), .A2(net_141) );
XNOR2_X1 inst_40 ( .ZN(net_47), .A(net_46), .B(net_36) );
NAND2_X1 inst_162 ( .ZN(net_29), .A2(net_27), .A1(n221) );
NAND2_X1 inst_167 ( .ZN(net_130), .A1(net_86), .A2(net_85) );
NOR3_X1 inst_93 ( .ZN(net_156), .A1(net_134), .A3(net_132), .A2(net_130) );
NOR4_X1 inst_81 ( .ZN(net_136), .A1(net_116), .A4(net_102), .A2(net_87), .A3(net_86) );
NOR3_X1 inst_95 ( .ZN(net_169), .A1(net_166), .A2(net_165), .A3(net_164) );
XOR2_X1 inst_1 ( .Z(net_6), .A(n119), .B(n116) );
XNOR2_X1 inst_72 ( .ZN(net_195), .B(net_190), .A(net_59) );
NAND4_X1 inst_139 ( .ZN(net_164), .A1(net_160), .A3(net_151), .A4(net_150), .A2(net_138) );
NAND2_X1 inst_155 ( .A2(net_172), .ZN(net_9), .A1(n227) );
XNOR2_X1 inst_59 ( .B(net_150), .ZN(n36), .A(n134) );
NOR2_X1 inst_135 ( .A2(net_196), .A1(net_154), .ZN(n75) );
INV_X1 inst_196 ( .ZN(net_133), .A(net_90) );
XNOR2_X1 inst_44 ( .ZN(net_61), .B(net_47), .A(net_39) );
XNOR2_X1 inst_55 ( .ZN(net_86), .A(net_73), .B(net_62) );
NAND2_X1 inst_174 ( .ZN(net_128), .A1(net_110), .A2(net_109) );
NOR2_X1 inst_115 ( .A1(net_134), .ZN(net_95), .A2(net_89) );
XNOR2_X1 inst_37 ( .ZN(net_38), .A(net_13), .B(n131) );
AND3_X4 inst_210 ( .A2(net_172), .ZN(net_121), .A3(net_24), .A1(n952) );
NAND3_X1 inst_148 ( .A3(net_180), .ZN(net_178), .A1(n902), .A2(n478) );
NAND2_X1 inst_164 ( .ZN(net_89), .A1(net_86), .A2(net_80) );
INV_X1 inst_191 ( .ZN(net_116), .A(net_29) );
XOR2_X1 inst_5 ( .Z(net_12), .A(n122), .B(n113) );
NAND2_X1 inst_157 ( .A2(net_172), .ZN(net_18), .A1(n224) );
NOR3_X1 inst_84 ( .ZN(net_19), .A2(net_1), .A3(n953), .A1(n237) );
XNOR2_X1 inst_51 ( .ZN(net_74), .A(net_54), .B(net_37) );
NAND3_X1 inst_142 ( .ZN(net_30), .A1(net_24), .A3(n953), .A2(n902) );
OR2_X4 inst_80 ( .A1(net_134), .ZN(net_123), .A2(net_93) );
NAND2_X1 inst_173 ( .ZN(net_132), .A2(net_109), .A1(net_108) );
NOR2_X1 inst_105 ( .ZN(net_66), .A2(net_65), .A1(net_8) );
AND2_X4 inst_213 ( .A2(net_53), .ZN(net_51), .A1(net_50) );
XNOR2_X1 inst_68 ( .ZN(net_189), .A(net_179), .B(net_77) );
AND2_X4 inst_216 ( .A1(net_69), .ZN(net_68), .A2(net_67) );
OR2_X4 inst_78 ( .ZN(net_127), .A1(net_86), .A2(net_80) );
XNOR2_X1 inst_42 ( .ZN(net_53), .A(net_44), .B(net_38) );
NAND2_X1 inst_175 ( .ZN(net_147), .A1(net_129), .A2(net_118) );
XNOR2_X1 inst_53 ( .ZN(net_77), .A(net_55), .B(net_35) );
INV_X1 inst_205 ( .ZN(net_170), .A(net_169) );
NAND2_X1 inst_177 ( .ZN(net_151), .A1(net_129), .A2(net_104) );
NAND2_X1 inst_183 ( .ZN(net_173), .A1(net_172), .A2(net_168) );
NOR2_X1 inst_133 ( .A1(net_194), .A2(net_193), .ZN(n51) );
XOR2_X1 inst_26 ( .A(net_159), .Z(n6), .B(n104) );
NAND3_X1 inst_151 ( .ZN(net_182), .A3(net_180), .A1(n902), .A2(n469) );
NOR2_X1 inst_112 ( .ZN(net_84), .A1(net_83), .A2(net_82) );
XNOR2_X1 inst_64 ( .B(net_176), .A(net_22), .ZN(n72) );
NOR2_X1 inst_107 ( .ZN(net_75), .A2(net_74), .A1(n902) );
XNOR2_X1 inst_67 ( .ZN(net_188), .A(net_178), .B(net_63) );
NAND2_X1 inst_181 ( .ZN(net_166), .A1(net_148), .A2(net_147) );
NOR2_X1 inst_127 ( .ZN(net_185), .A2(net_184), .A1(net_73) );
XNOR2_X1 inst_70 ( .ZN(net_192), .A(net_185), .B(net_61) );
INV_X1 inst_186 ( .ZN(net_172), .A(n953) );
NOR2_X1 inst_129 ( .A1(net_194), .A2(net_189), .ZN(n60) );
NOR3_X1 inst_92 ( .ZN(net_139), .A1(net_135), .A2(net_133), .A3(net_128) );
XNOR2_X1 inst_29 ( .ZN(net_42), .A(net_6), .B(n113) );
INV_X1 inst_189 ( .ZN(net_1), .A(n214) );
XOR2_X1 inst_17 ( .Z(net_100), .A(net_79), .B(net_76) );
XOR2_X1 inst_11 ( .Z(net_39), .B(net_25), .A(n137) );
NAND3_X1 inst_146 ( .A3(net_143), .ZN(net_122), .A2(net_121), .A1(net_119) );
INV_X1 inst_188 ( .ZN(net_14), .A(n902) );
XOR2_X1 inst_14 ( .Z(net_80), .B(net_72), .A(n472) );
INV_X1 inst_202 ( .ZN(net_104), .A(net_103) );
AND4_X1 inst_206 ( .A1(net_121), .A4(net_113), .ZN(net_112), .A2(net_106), .A3(net_105) );
INV_X1 inst_187 ( .ZN(net_0), .A(n210) );
NOR2_X1 inst_122 ( .ZN(net_141), .A2(net_132), .A1(net_127) );
XNOR2_X1 inst_31 ( .ZN(net_34), .A(net_5), .B(n107) );
XOR2_X1 inst_25 ( .A(net_158), .Z(n9), .B(n107) );
NOR2_X1 inst_126 ( .ZN(net_171), .A2(net_167), .A1(n953) );
NAND2_X1 inst_158 ( .ZN(net_20), .A2(net_3), .A1(n953) );
NAND3_X1 inst_141 ( .A3(net_172), .ZN(net_23), .A1(n234), .A2(n217) );
XNOR2_X1 inst_62 ( .B(net_160), .ZN(n30), .A(n128) );
INV_X1 inst_200 ( .ZN(net_98), .A(net_97) );
NOR2_X1 inst_110 ( .ZN(net_142), .A2(net_83), .A1(net_81) );
OR3_X2 inst_74 ( .ZN(net_131), .A3(net_130), .A2(net_124), .A1(net_123) );
XNOR2_X1 inst_57 ( .B(net_147), .ZN(n42), .A(n140) );
XNOR2_X1 inst_35 ( .ZN(net_37), .A(net_15), .B(net_9) );
NOR2_X1 inst_99 ( .ZN(net_194), .A1(net_172), .A2(n952) );
XNOR2_X1 inst_48 ( .ZN(net_63), .A(net_48), .B(net_23) );
XNOR2_X1 inst_69 ( .ZN(net_190), .B(net_181), .A(net_58) );
XNOR2_X1 inst_46 ( .ZN(net_58), .B(net_53), .A(net_42) );
NOR4_X1 inst_82 ( .ZN(net_161), .A2(net_159), .A4(net_158), .A3(net_156), .A1(net_155) );
NAND4_X1 inst_136 ( .ZN(net_102), .A1(net_101), .A4(net_100), .A3(net_85), .A2(net_84) );
XNOR2_X1 inst_30 ( .ZN(net_33), .A(net_16), .B(n134) );
NOR2_X1 inst_102 ( .A2(net_53), .ZN(net_52), .A1(net_50) );
NOR2_X1 inst_108 ( .ZN(net_78), .A2(net_77), .A1(n902) );
NAND2_X1 inst_165 ( .ZN(net_135), .A1(net_87), .A2(net_83) );
XNOR2_X1 inst_32 ( .ZN(net_35), .A(net_12), .B(n104) );
XOR2_X1 inst_22 ( .A(net_155), .Z(n3), .B(n101) );
NAND3_X1 inst_144 ( .ZN(net_138), .A3(net_118), .A1(net_110), .A2(net_107) );
XNOR2_X1 inst_34 ( .ZN(net_41), .A(net_4), .B(n101) );
XOR2_X1 inst_12 ( .Z(net_43), .A(net_42), .B(net_41) );
INV_X1 inst_195 ( .A(net_114), .ZN(net_93) );
XNOR2_X1 inst_56 ( .B(net_138), .ZN(n27), .A(n125) );
XNOR2_X1 inst_71 ( .ZN(net_193), .A(net_186), .B(net_71) );
XOR2_X1 inst_21 ( .A(net_139), .Z(n45), .B(n143) );
NOR2_X1 inst_104 ( .ZN(net_64), .A2(net_63), .A1(n902) );
XNOR2_X1 inst_60 ( .B(net_151), .ZN(n33), .A(n131) );
AND2_X4 inst_215 ( .ZN(net_62), .A2(net_61), .A1(net_14) );
NAND2_X1 inst_169 ( .A1(net_142), .ZN(net_97), .A2(net_90) );
NAND2_X1 inst_168 ( .A1(net_101), .A2(net_100), .ZN(net_91) );
NOR2_X1 inst_97 ( .A1(net_172), .ZN(net_8), .A2(n898) );
NAND2_X1 inst_161 ( .ZN(net_79), .A2(net_21), .A1(n210) );
NOR2_X1 inst_124 ( .ZN(net_163), .A1(net_146), .A2(net_145) );
XOR2_X1 inst_18 ( .A(net_152), .Z(n21), .B(n119) );
XOR2_X1 inst_16 ( .Z(net_83), .A(net_78), .B(n475) );
AND4_X1 inst_208 ( .ZN(net_191), .A2(net_183), .A4(net_149), .A1(net_120), .A3(net_117) );
NOR3_X1 inst_88 ( .ZN(net_76), .A1(net_70), .A3(net_68), .A2(n902) );
AND2_X4 inst_220 ( .ZN(net_158), .A1(net_142), .A2(net_141) );
XOR2_X1 inst_3 ( .Z(net_10), .A(n122), .B(n110) );
NAND2_X1 inst_156 ( .ZN(net_27), .A2(net_14), .A1(n234) );
XOR2_X1 inst_9 ( .Z(net_17), .A(n146), .B(n143) );
NOR2_X1 inst_113 ( .A2(net_100), .ZN(net_94), .A1(net_28) );
NAND2_X1 inst_170 ( .A1(net_143), .ZN(net_103), .A2(net_90) );
INV_X1 inst_198 ( .ZN(net_105), .A(net_91) );
XNOR2_X1 inst_50 ( .ZN(net_55), .B(net_49), .A(net_46) );
NAND4_X1 inst_137 ( .A1(net_121), .ZN(net_117), .A3(net_116), .A4(net_113), .A2(net_92) );
INV_X1 inst_199 ( .ZN(net_96), .A(net_95) );
XNOR2_X1 inst_41 ( .ZN(net_48), .B(net_34), .A(net_33) );
NOR2_X1 inst_130 ( .A1(net_194), .A2(net_188), .ZN(n63) );
NOR3_X1 inst_91 ( .ZN(net_153), .A1(net_135), .A2(net_127), .A3(net_126) );
NOR2_X1 inst_132 ( .A1(net_194), .A2(net_192), .ZN(n66) );
NAND3_X1 inst_143 ( .A3(net_172), .ZN(net_25), .A1(n234), .A2(n221) );
NAND2_X1 inst_176 ( .ZN(net_148), .A1(net_129), .A2(net_95) );
NAND2_X1 inst_152 ( .ZN(net_2), .A2(n900), .A1(n227) );
XNOR2_X1 inst_58 ( .B(net_148), .ZN(n39), .A(n137) );
XNOR2_X1 inst_36 ( .ZN(net_44), .A(net_17), .B(n128) );
NAND3_X1 inst_147 ( .ZN(net_168), .A2(net_163), .A1(net_162), .A3(net_161) );
NOR3_X1 inst_87 ( .ZN(net_72), .A3(net_60), .A1(net_57), .A2(n902) );
XNOR2_X1 inst_61 ( .B(net_157), .ZN(n48), .A(n146) );
INV_X1 inst_203 ( .ZN(net_137), .A(net_136) );
XNOR2_X1 inst_45 ( .ZN(net_54), .B(net_53), .A(net_41) );
NOR3_X1 inst_96 ( .ZN(net_167), .A2(net_166), .A1(net_165), .A3(net_164) );
AND3_X4 inst_212 ( .ZN(net_174), .A1(net_163), .A2(net_162), .A3(net_161) );
NOR2_X1 inst_101 ( .ZN(net_32), .A2(net_30), .A1(n898) );
XOR2_X1 inst_0 ( .Z(net_5), .A(n122), .B(n116) );
NAND2_X1 inst_184 ( .ZN(net_180), .A2(net_174), .A1(net_169) );
XOR2_X1 inst_10 ( .B(net_50), .Z(net_46), .A(n146) );
XOR2_X1 inst_4 ( .Z(net_50), .A(n140), .B(n125) );
XNOR2_X1 inst_65 ( .B(net_177), .A(net_20), .ZN(n69) );
NAND2_X1 inst_178 ( .ZN(net_150), .A1(net_129), .A2(net_98) );
NOR3_X1 inst_89 ( .A3(net_127), .ZN(net_119), .A2(net_93), .A1(net_91) );
XNOR2_X1 inst_28 ( .ZN(net_4), .A(n107), .B(n104) );
NOR2_X1 inst_111 ( .ZN(net_90), .A1(net_86), .A2(net_85) );
XNOR2_X1 inst_66 ( .ZN(net_187), .A(net_182), .B(net_74) );
NOR2_X1 inst_117 ( .ZN(net_92), .A2(net_91), .A1(net_82) );
NOR2_X1 inst_98 ( .A1(net_172), .ZN(net_11), .A2(n900) );
INV_X1 inst_190 ( .A(net_101), .ZN(net_28) );
XNOR2_X1 inst_63 ( .ZN(net_177), .B(net_173), .A(net_66) );
XOR2_X1 inst_7 ( .Z(net_15), .A(n140), .B(n110) );
INV_X1 inst_204 ( .ZN(net_140), .A(net_139) );
NAND2_X1 inst_185 ( .ZN(net_184), .A2(net_180), .A1(n902) );
NAND2_X1 inst_182 ( .ZN(net_165), .A1(net_157), .A2(net_140) );
XNOR2_X1 inst_49 ( .ZN(net_69), .A(net_45), .B(net_18) );
NOR2_X1 inst_120 ( .ZN(net_145), .A1(net_126), .A2(net_97) );
NAND2_X1 inst_154 ( .ZN(net_24), .A2(n237), .A1(n234) );
XOR2_X1 inst_13 ( .Z(net_45), .B(net_44), .A(n125) );
NOR2_X1 inst_119 ( .ZN(net_152), .A1(net_126), .A2(net_96) );
OR2_X4 inst_75 ( .ZN(net_21), .A2(n902), .A1(n237) );
INV_X1 inst_192 ( .ZN(net_67), .A(net_65) );
NAND2_X1 inst_166 ( .ZN(net_88), .A2(net_83), .A1(net_81) );
NOR2_X1 inst_116 ( .A1(net_134), .A2(net_127), .ZN(net_113) );
NAND2_X1 inst_163 ( .ZN(net_73), .A2(net_27), .A1(n217) );
NOR3_X1 inst_85 ( .ZN(net_26), .A2(net_0), .A3(n953), .A1(n237) );
XNOR2_X1 inst_54 ( .ZN(net_81), .A(net_64), .B(n478) );
OR2_X4 inst_79 ( .ZN(net_134), .A1(net_87), .A2(net_83) );
NOR2_X1 inst_109 ( .A1(net_116), .ZN(net_114), .A2(net_82) );
NOR2_X1 inst_106 ( .ZN(net_70), .A1(net_69), .A2(net_67) );
AND2_X4 inst_219 ( .ZN(net_109), .A1(net_106), .A2(net_94) );
INV_X1 inst_201 ( .A(net_113), .ZN(net_99) );
INV_X1 inst_193 ( .ZN(net_87), .A(net_81) );
NAND3_X1 inst_149 ( .A3(net_180), .ZN(net_179), .A1(n902), .A2(n475) );
XNOR2_X1 inst_43 ( .ZN(net_49), .A(net_40), .B(n131) );
XNOR2_X1 inst_39 ( .ZN(net_40), .A(net_19), .B(n143) );
NOR2_X1 inst_128 ( .ZN(net_186), .A2(net_184), .A1(net_79) );
OR3_X2 inst_73 ( .A2(net_133), .ZN(net_125), .A1(net_124), .A3(net_123) );
AND2_X4 inst_217 ( .ZN(net_106), .A2(net_82), .A1(net_29) );
XOR2_X1 inst_23 ( .A(net_153), .Z(n24), .B(n122) );
NAND2_X1 inst_171 ( .ZN(net_124), .A1(net_121), .A2(net_105) );
OR2_X4 inst_77 ( .A1(net_121), .ZN(net_108), .A2(net_32) );
NOR3_X1 inst_94 ( .ZN(net_154), .A3(net_136), .A2(n953), .A1(n952) );

endmodule
