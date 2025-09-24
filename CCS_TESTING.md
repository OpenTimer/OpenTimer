# CCS Parser Testing Guide

## Quick Start

```bash
# Build and run quick test
make test

# Run comprehensive test suite
make test-all

# Test with your own CCS library file
./test_ccs_quick /path/to/your/library.lib
```

## Test Programs

### 1. `test_ccs_quick` - Quick Validation Test
Fast test to verify CCS parser is working correctly.

**What it checks:**
- Library loads without errors
- Output_current tables are found
- Multiple values per table are parsed
- Shows sample data

**Usage:**
```bash
./test_ccs_quick [library_file]
```

**Expected output:**
```
✓ Library parsed successfully!
Results:
  Timing arcs with output_current: 5
  Total tables: 640
  Total values parsed: 13808
  Average values per table: 21
✓✓✓ SUCCESS: CCS parser is working correctly! ✓✓✓
```

### 2. `test_ccs_comprehensive` - Full Test Suite
Comprehensive validation with multiple test cases and assertions.

**Test cases:**
1. **Basic Parsing** - Verifies tables are found and counted correctly
2. **Value Completeness** - Checks all values are extracted (not just first one)
3. **Data Integrity** - Validates current values are reasonable
4. **CCSN Stages** - Tests CCSN-specific parsing

**Usage:**
```bash
./test_ccs_comprehensive [additional_lib_file]
```

### 3. `test_output_current` - Detailed Analysis
Original test that shows detailed statistics for each library.

**Usage:**
```bash
./test_output_current
```

## What Makes a Successful Test?

### ✓ Good Results:
- **Multiple values per table** (average > 10)
- **Index_3 size matches value count**
- **Reasonable current values** (-100mA to +100mA typical)
- **No parsing errors**

### ✗ Bad Results:
- **Only 1 value per table** (parser not working)
- **Zero tables found** (output_current not detected)
- **Parsing errors** (file format issues)

## Test Files Included

| File | Description | Expected Results |
|------|-------------|------------------|
| `test_ccs_minimal.lib` | Single cell, minimal CCS | 640 tables, ~14 values each |
| `test_ccs_10cells.lib` | 10 cells, medium complexity | 3,712 tables, ~22 values each |
| `test_ccs_50cells.lib` | 50 cells | ~18K tables |
| `test_ccs_100cells.lib` | 100 cells | ~37K tables |
| `test_ccs_500cells.lib` | 500 cells, stress test | ~185K tables |

## Testing Your Own Files

### Quick validation:
```bash
./test_ccs_quick /path/to/your/file.lib
```

### Look for:
1. **"SUCCESS" message** - Parser working correctly
2. **Average values per table > 1** - Multi-value parsing working
3. **Sample table shows multiple values** - Data integrity good

### If you see warnings:
- **"Tables found but may have incomplete values"** - Parser found tables but values might be truncated
- **"No output_current tables found"** - File might not have CCS data or different format

## Troubleshooting

### Common Issues:

1. **Compilation errors:**
   ```bash
   make clean
   make all
   ```

2. **File not found:**
   - Check file path is correct
   - Use absolute paths for files outside current directory

3. **Only 1 value per table:**
   - This was the original bug - make sure you have the latest ccs_parser.cpp
   - Rebuild: `make clean && make all`

4. **No tables found:**
   - File might use different CCS format
   - Check if file has `output_current_rise` or `output_current_fall` blocks

## Performance Benchmarks

| Library Size | Parse Time | Memory Usage |
|-------------|------------|--------------|
| 1 cell (700KB) | <1s | ~10MB |
| 10 cells (4MB) | <1s | ~50MB |
| 100 cells (116MB) | ~2s | ~500MB |
| 1154 cells/TSMC7 (1.2GB) | ~10s | ~5GB |

## Integration Testing

To verify the parser works with your codebase:

```cpp
#include <ot/liberty/celllib.hpp>

ot::Celllib lib;
lib.read("your_ccs_file.lib");

// Access output_current data
for (const auto& [name, cell] : lib.cells) {
    for (const auto& [pin_name, pin] : cell.cellpins) {
        for (const auto& timing : pin.timings) {
            // Access CCS current waveforms
            for (const auto& lut : timing.output_current_rise) {
                // lut.table contains current values
                // lut.indices3 contains time points
            }
        }
    }
}
```

## Success Criteria

The CCS parser is working correctly when:
- ✓ All test programs compile without errors
- ✓ `test_ccs_quick` shows "SUCCESS"
- ✓ Average values per table > 10
- ✓ `test_ccs_comprehensive` shows "ALL TESTS PASSED"
- ✓ Your own CCS files parse without errors