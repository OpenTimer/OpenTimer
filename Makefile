# Makefile for CCS Parser Tests
CXX = g++
CXXFLAGS = -std=c++17 -I. -O2
LIBS = ot/liberty/*.cpp ot/unit/*.cpp

# Test programs
TESTS = test_ccs_quick test_ccs_comprehensive test_output_current

.PHONY: all clean test

all: $(TESTS)

test_ccs_quick: test_ccs_quick.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)

test_ccs_comprehensive: test_ccs_comprehensive.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)

test_output_current: test_output_current.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)

# Run quick test
test: test_ccs_quick
	./test_ccs_quick

# Run comprehensive tests
test-all: test_ccs_comprehensive
	./test_ccs_comprehensive

# Clean up
clean:
	rm -f $(TESTS) *.o

# Help
help:
	@echo "CCS Parser Test Suite"
	@echo "===================="
	@echo "make all              - Build all test programs"
	@echo "make test            - Run quick test"
	@echo "make test-all        - Run comprehensive tests"
	@echo "make clean           - Clean build files"
	@echo ""
	@echo "Individual tests:"
	@echo "  ./test_ccs_quick [lib_file]           - Quick test (default: test_ccs_minimal.lib)"
	@echo "  ./test_ccs_comprehensive               - Full test suite"
	@echo "  ./test_output_current                  - Original output_current test"
	@echo ""
	@echo "Test with your own file:"
	@echo "  ./test_ccs_quick path/to/your/file.lib"