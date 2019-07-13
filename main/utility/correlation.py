import math
import sys
import os
import re
import numpy as np
from scipy import stats
import matplotlib.pyplot as plt

#ot patterns
OT_REPORT = re.compile("^report_timing.*")
ENDPOINT_RE = re.compile("^Endpoint:\s*(\S+)")
BEGIN_RE = re.compile("Beginpoint:\s*(\S+)")
RAT_RE = re.compile(".*Required Time\s*(\S+)")
AT_RE = re.compile(".*Arrival Time\s*(\S+)")
SLACK_RE = re.compile(".*Slack Time\s*(\S+)")
POINT_RE = re.compile("\S+\s+(\S+)\s+(v|\^)\s+(\S+)")
#POINT_RE = re.compile("\s+(v|\^)\s+(\S+)")

#osta pattens
OSTA_REPORT = re.compile("^report_checks.*")
OSTA_START = re.compile("^Startpoint:\s*(\S+)\s+.*")
OSTA_END = re.compile("^Endpoint:\s*(\S+)\s+.*")
OSTA_SLACK = re.compile("\s*(\S+)\s*slack.*")
OSTA_POINT_RE = re.compile(".+\s+(\S+)\s+(v|\^)\s+(\S+)")

global ot_first, osta_first
ot_first = True
osta_first = True

def osta_get_path(f):

  global osta_first

  summary = []
  nodes = []

  line = f.readline()
  m = re.match(OSTA_START, line)

  # Search the "Startpoint"
  while m is None:
    if len(line) is 0:
      return [], []
    elif re.match(OSTA_REPORT, line):
      if osta_first:
        osta_first = False
        #print "First query ", line
      else:
        #print "Next query", line
        return [], []

    line = f.readline()
    m = re.match(OSTA_START, line)

  start = m.group(1)
  end = ""
  line = f.readline()

  # Search the "Endpoint"
  m = re.match(OSTA_END, line)
  while m is None:
    line = f.readline()
    m = re.match(OSTA_END, line)

  if m:
    end = m.group(1)
  else:
    print(line)
    print("OSTA Invalid Endpoint format")
    sys.exit(1)
 
  #f.readline()
  
  # Search the line before points "----------------------"
  line = f.readline()
  while "-----" not in line:
    line = f.readline()

  ##dummy lines
  #for i in range(7):
  #  line = f.readline()  
  #line = f.readline()

  line = f.readline()

  # Read points Format: (Delay, Time, Description)
  m = re.match(OSTA_POINT_RE, line)
  while m:
    point = [m.group(1), m.group(2), m.group(3)]
    nodes.append(point) 
    line = f.readline()
    m = re.match(OSTA_POINT_RE, line)

  line = f.readline()

  # Search for "slack"
  m = re.match(OSTA_SLACK, line)
  while m is None:
    line = f.readline()
    m = re.match(OSTA_SLACK, line)

  slack = float(m.group(1)) 

  f.readline()
  f.readline()

  summary = [end, start, slack]

  return summary, nodes  
 

def ot_get_path(f):

  global ot_first

  summary = []
  nodes = []

  line = f.readline()
  if len(line) is 0:
    return [], []
  elif re.match(OT_REPORT, line):
    if ot_first:
      ot_first = False
      line = f.readline()
      #print "First query ", line
    else:
      #print "Next query ", line
      return [], []

  m = re.match(ENDPOINT_RE, line)
  if m:
    summary.append(m.group(1))
  else:
    print(line)
    print("OT Invalid Endpoint format")
    sys.exit(1)

  line = f.readline() 
  m = re.match(BEGIN_RE, line)
  if m:
    summary.append(m.group(1))
  else:
    print("Invalid Beginpoint format")
    sys.exit(1)

  line = f.readline() #required time 
  line = f.readline() #arrival time
  line = f.readline() #slack time
  m = re.match(SLACK_RE, line)
  if m:
    slack = float(m.group(1))
  else:
    print(line)
    print("Invlid Slack Format")
    sys.exit(1)
  summary.append(slack)

  line = f.readline()
  m = re.match(POINT_RE, line)
  while m:
    point = [m.group(1), m.group(2), m.group(3)]
    nodes.append(point)
    line = f.readline()
    m = re.match(POINT_RE, line)

  return summary, nodes

def osta_kpaths(f):

  slacks = []
  kpaths = []

  while True:
    summary, nodes = osta_get_path(f)
    if len(summary) is 0:
      break  
    slacks.append(summary[-1])
    kpaths.append(nodes)
  
  return slacks, kpaths 


def ot_kpaths(f):

  slacks = []
  kpaths = []

  while True:
    summary, nodes = ot_get_path(f)
    if len(summary) is 0:
      break  
    slacks.append(summary[-1])
    kpaths.append(nodes)
  
  return slacks, kpaths 

def cmp_path(ot_trace, osta_trace):
  
  j = 0
  for i in range(len(ot_trace)): 
    if ot_trace[i][1] == osta_trace[j][1] and ot_trace[i][2] == osta_trace[j][2]:
      j+=1

  #print j, len(osta_trace) 
  if j != len(osta_trace):
    return False

  return True
  
def same_path_rate(ot_nodes, osta_nodes):

  k = len(osta_nodes)
  lookup = np.zeros(len(ot_nodes))

  cnt = 0.0
  for j in range(k):
    for i in range(len(ot_nodes)):
      #if lookup[i] is 1:
      #  continue

      if cmp_path(ot_nodes[i], osta_nodes[j]):
        lookup[i] = 1
        cnt += 1.0
        break
      #else:
      #  print "----------OT_PATH--------------"
      #  print ot_nodes[i]
      #  print "----------OST_PATH-------------"
      #  print osta_nodes[i]

  return cnt/float(k) 

def hist(ot_arr, osta_arr, tgt):

  A = np.array(ot_arr)
  B = np.array(osta_arr) 

  bin_num = 100

  ot_histfile = open(tgt+"/"+tgt+"_ot_hist.dat", 'w')
  osta_histfile = open(tgt+"/"+tgt+"_osta_hist.dat", 'w')
  ot_hist, ot_bins = np.histogram(A, bins=bin_num) 
  osta_hist, osta_bins = np.histogram(B, bins=bin_num)

  for i in range(bin_num):
    ot_histfile.write(("%.3f" % ot_bins[i]) + " " + str(ot_hist[i]) +"\n")
    osta_histfile.write(("%.3f" % osta_bins[i]) + " " + str(osta_hist[i]) +"\n")


def kpaths(otfile, ostafile, tgt):

  ot = open(otfile, 'r')
  osta = open(ostafile, 'r')

  corrfile = open(tgt+"/"+tgt+"_corr.csv", 'w') 
  r2file = open(tgt+"/"+tgt+"_r2.csv", 'w')
  #pathratefile = open(tgt+"/"+tgt+"_rate.csv", 'w')

  corrfile.write("ite,\trate\n")
  r2file.write("ite,\trate\n")
  #pathratefile.write("ite,\trate\n")

  path_cnt = 0
  
  #path summary and path nodes
  ot_slacks = []
  ot_nodes = []
  osta_slacks = []
  osta_nodes = []

  ot_arr = []
  osta_arr = []
 
  while True:
    ot_slacks, ot_nodes = ot_kpaths(ot)
    osta_slacks, osta_nodes = osta_kpaths(osta)


    if len(ot_slacks) is 0 or len(osta_slacks) is 0:
    #if path_cnt == 1200:
      break

    
    #print ot_slacks
    #print osta_slacks
    if len(ot_slacks) != len(osta_slacks):
      print("Number of paths different ", len(ot_slacks), len(osta_slacks))

    num = min(len(ot_slacks),len(osta_slacks))

    if num != 0:
      path_cnt += 1
    else:
      print("No paths ", len(ot_slacks), len(osta_slacks))
      continue
 
    x = np.array(ot_slacks[:num])
    y = np.array(osta_slacks[:num])

    corr = 1.0
    r2 = 1.0
    if np.var(x) != 0.0 and np.var(y) != 0.0:
    #print x, y
      corr = np.corrcoef(x, y)[0][1]
      slope, inter, r_val, p_val, std_err = stats.linregress(x, y)
      r2 = r_val ** 2
    #pathrate = same_path_rate(ot_nodes, osta_nodes)

    print(path_cnt, corr, r2)

    corrfile.write(str(path_cnt)+",\t"+str(corr)+"\n")
    r2file.write(str(path_cnt)+",\t"+str(r2)+"\n")
   # pathratefile.write(str(path_cnt)+",\t"+str(pathrate)+"\n")

    for i in range(num):
      ot_arr.append(ot_slacks[i])
      osta_arr.append(osta_slacks[i])

    #for i in range(min(len(ot_nodes), len(osta_nodes))):
    #  print "----------OpenTimer------------------"
    #  print ot_nodes[i]
    #  print "----------OpenSTA--------------------"
    #  print osta_nodes[i]
    #  print "-------------------------------------" 
   
  hist(ot_arr, osta_arr, tgt) 

# python3 correlation.py OpenTimer_output OpenSTA_output an_empty_folder
if __name__ == "__main__":
  otfile = sys.argv[1]
  ostafile = sys.argv[2]
  tgt = sys.argv[3]
  kpaths(otfile, ostafile, tgt)



