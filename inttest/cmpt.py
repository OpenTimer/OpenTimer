#!/usr/bin/env python
import math
import sys
import os

# Procedure: compare_timing
# compare the two timing outputs (numerics)
def compare_timing(output, golden):

  O = [line.strip() for line in open(output)]
  G = [line.strip() for line in open(golden)]

  output_timing = []
  golden_timing = []
  
  # extract the output data
  for line in O:
    token = line.split();
    if len(token) == 1:
      output_timing.append(float(token[0]))
    elif len(token) == 6:
      output_timing.append(float(token[3]))

  # extract the golden data
  for line in G:
    token = line.split();
    if len(token) == 1:
      golden_timing.append(float(token[0]))
    elif len(token) == 6:
      golden_timing.append(float(token[3]))

  # extract the path data

  if len(output_timing) != len(golden_timing):
    print("output timing length doesn't match golden")
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

