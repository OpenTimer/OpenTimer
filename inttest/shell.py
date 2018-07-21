#!/usr/bin/env python
import os
import subprocess
import sys
import math
import cmpt

# Usage: python shell.py ot-shell benchmark/ circuit

exe=sys.argv[1]
dir=sys.argv[2]
tgt=sys.argv[3]

os.chdir(dir + '/' + tgt)

shell   = tgt + ".shell" 
golden  = tgt + ".output"
output  = ".output"

# execute the shell to generate timing report
with open(shell) as f:
  subprocess.call([exe, "-o", output], stdin=f)

# compare the output with the golden
cmpt.compare_timing(output, golden)

