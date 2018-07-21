#!/usr/bin/env python
import math
import sys
import os

# Procedure: compare_timing
# compare the two timing outputs (numerics)
def compare_timing(output, golden):

  with open(golden, 'r') as f:
    golden_timing = [float(line.rstrip()) for line in f]
  
  with open(output, 'r') as f:
    output_timing = [float(line.rstrip()) for line in f]
  
  if len(output_timing) != len(golden_timing):
    print("output length doesn't match golden")
    sys.exit(1)
  
  for i in range(len(output_timing)):
  
    if math.isnan(golden_timing[i]):
      continue
  
    if math.isnan(output_timing[i]):
      if(abs(abs(golden_timing[i]) - 987654.0) > 0.1):
        print("Line", i+1, ":", output_timing[i], "(output) vs", golden_timing[i], "(golden)")
        sys.exit(1)
    else:
      if (abs(output_timing[i] - golden_timing[i]) > 0.1):
        print("Line", i+1, ":", output_timing[i], "(output) vs", golden_timing[i], "(golden)")
        sys.exit(1)
  
  if os.path.isfile(output):
    os.remove(output)

