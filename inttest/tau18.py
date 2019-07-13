#!/usr/bin/env python
import os
import subprocess
import sys
import math
import cmpt18
import zipfile

# Usage: python inttest.py ot-tau18 benchmark/ circuit

exe=sys.argv[1]
dir=sys.argv[2]
tgt=sys.argv[3]

os.chdir(dir + '/' + tgt)

tau2018 = tgt + ".tau2018"
timing  = tgt + ".timing"
ops     = tgt + ".ops"
golden  = tgt + ".output"
output  = ".output"

# execute the tau18 binary to generate timing report
subprocess.call([exe, tau2018, timing, ops, output])

# Unzip golden file 
golden_zip = zipfile.ZipFile(golden + ".zip", 'r')
golden_zip.extractall(".")
golden_zip.close()

# compare the output with the golden
cmpt18.compare_timing(output, golden)

# Remove golden file
if os.path.isfile(golden):
  os.remove(golden)

