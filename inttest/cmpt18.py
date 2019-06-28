#!/usr/bin/env python
import math
import sys
import os

def is_equal(output, golden, item_name):
  if output != golden:
    print(item_name, " doesn't match golden")
    sys.exit(1)

def is_numerical_equal(output, golden, item_name):
  if math.fabs(output-golden) > 0.1:
    print(item_name, " doesn't match golden")
    sys.exit(1)

# Procedure: compare_timing
# compare the two path timing outputs 
def compare_timing(output, golden):

  O = [line.strip() for line in open(output)]
  G = [line.strip() for line in open(golden)]

  endpoint = []
  beginpoint = []
  req_time = []
  arr_time = []
  slack = []
  
  # extract the output data
  for line in O:

    # Skip empty lines
    if len(line.strip()) == 0:
      continue 

    # Skip comment line
    if line[0] == '#':
      continue

    # Remove leading space
    line = line.lstrip()

    token = line.split()

    if token[0] == "Endpoint":
      endpoint.append(token[-1])
    elif token[0] == "Beginpoint":
      beginpoint.append(token[-1])
    elif token[1] == "Required":
      req_time.append(float(token[-1]))
    elif token[1] == "Arrival":
      arr_time.append(float(token[-1]))
    elif token[1] == "Slack":
      slack.append(float(token[-1]))

  num_checked_paths = 0
  
  ## extract the golden data
  for line in G:
    # Skip empty lines
    if len(line.strip()) == 0:
      continue 

    # Skip comment line
    if line[0] == '#':
      continue

    # Remove leading space
    line = line.lstrip()
    token = line.split()

    if token[0] == "Endpoint":
      is_equal(endpoint[num_checked_paths], token[-1], "end point")
    elif token[0] == "Beginpoint":
      is_equal(beginpoint[num_checked_paths], token[-1], "begin point")
    elif token[1] == "Required":
      is_numerical_equal(req_time[num_checked_paths], float(token[-1]), "required_time")
    elif token[1] == "Arrival":
      is_numerical_equal(arr_time[num_checked_paths], float(token[-1]), "arrival_time")
    elif token[1] == "Slack":
      is_numerical_equal(slack[num_checked_paths], float(token[-1]), "slack")
      num_checked_paths += 1
      if num_checked_paths == len(slack):
        break

  if os.path.isfile(output):
    os.remove(output)

