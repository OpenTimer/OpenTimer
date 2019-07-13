# Note for tau18 folder

:exclamation: leon3mp_iccad verilog file is compressed as zip file. Uncompress the zip file before you test.

Those benchmarks are used to test OpenTimer's path-based analysis (PBA) functions: report_timing & report_timing_batch. 
The test file is /main/tau18/tau18.cpp. Compile OpenTimer and find the binary `ot-tau18` under bin folder. 
To run this binary:
```
ot-tau18 leon3mp_iccad.tau2018 leon3mp_iccad.timing leon3mp_iccad.ops OT_output
```

## Note for vga_lcd & leon3mp_iccad
These two benchmarks are from tau 2018 contest. Below are the setting for tau 2018 contest:
1. Each query only reports the **top 1** worst path.
2. Assume **ideal clock**.
3. The mode is **late mode (MAX)**.
4. For leon3mp_iccad, the slacks are **positive and lower than 40000**. So you need to set up the slack upper bound
   when doing PBA. 
5. We compare the outputs with the winner in tau 2018 contest: iTimer. **The golden files are compressed as zip files (*.output)**. 
   Use the file: /main/utility/tau18_cmp.cpp 
   This file can be directly compiled: 
```
  g++ -std=c++17 -O2 tau18_cmp.cpp -o verifier
```
   Run this file:  
```
 ./verifier OT_output iTimer_output
```

## Note for multi_paths_* benchmarks
Those benchmarks are generated from vga_lcd & leon3mp_iccad. To validate the outputs
we calculate the correlation coefficient between OT & OpenSTA:

1. Create an empty folder inside each benchmark folder, e. g. `mkdir vga_lcd` under multi_paths_vga_lcd
2. Run the `ot-tau18` to report timing paths and store them in a file.
3. Run the correlation.py: 
```
  python3 correlation.py OT_output OpenSTA_output vga_lcd. 
```
  You will see three numbers: `path count` `Pearson Correlation Coefficient` `Coefficient of determination (R2)`
4. The correlation should be close to 1

