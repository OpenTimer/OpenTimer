#!/usr/bin/env python
import os
import subprocess
import sys
import math
import cmpt

# Usage: python inttest.py ot-tau15 benchmark/ circuit

exe=sys.argv[1]
dir=sys.argv[2]
tgt=sys.argv[3]

os.chdir(dir + '/' + tgt)

tau2015 = tgt + ".tau2015"
timing  = tgt + ".timing"
ops     = tgt + ".ops"
golden  = tgt + ".output"
output  = ".output"

# execute the tau15 binary to generate timing report
subprocess.call([exe, tau2015, timing, ops, output])

# compare the output with the golden
cmpt.compare_timing(output, golden)






