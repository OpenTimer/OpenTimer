# General Questions

This page summarizes a list of general questions regarding the use of OpenTimer.

---

#### Q: What is OpenTimer?

**A:** OpenTimer is a [static timing analysis (STA)][STA] tool 
for very large scale integration (VLSI) systems.
It is written in modern C++ and operates on industry standard input formats (.sdc, .lib, .v, .spef).

#### Q: How is OpenTimer licensed? Will I have litigation risk in using OpenTimer?

**A:** OpenTimer is open under [MIT](../../LICENSE) license. 
It is completely free and non-viral for both academic and commercial use. 
However, the project does contain third-party tools and services that users should be aware of.
For example, the libraries we use in example and regression folders are subject to extra license agreement.
You will need to double check these [licenses](../../licenses).

#### Q: What industry formats do OpenTimer support?

**A:** OpenTimer takes (1) cell library (.lib), (2) verilog netlist (.v), (3) parasitics (.spef),
and (4) Synopsys design constraint (.sdc).
Additional formats from [TAU contest][TAU15] such as .timing are also admissible.

#### Q: Does OpenTimer do signal integrity (SI) analysis?

**A:** OpenTimer currently does not support SI analysis, but it is in our wish list.


* * *

[STA]:                   https://en.wikipedia.org/wiki/Static_timing_analysis
[DARPA IDEA]:            https://www.darpa.mil/news-events/2017-09-13
[TAU15]:                 https://sites.google.com/site/taucontest2015/resources
