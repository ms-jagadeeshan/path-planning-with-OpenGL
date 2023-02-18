# Compiler settings
CXX = g++
CXXFLAGS = -Wall -O2
LIBS = -lGL -lGLU -lglut -lGLEW
INC = -I ./include
BUILD = build

default: include/algo_info.hpp pathvizgl

pathvizgl: $(BUILD)/pathvizgl.o $(BUILD)/algorithms.o
	@echo "Linking..."
	$(CXX) -o $@  $(CXXFLAGS) $(INC) $^  $(LIBS)

$(BUILD)/pathvizgl.o: src/pathvizgl.cpp
	@echo "Compiling..."
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

$(BUILD)/algorithms.o: src/algorithm/algorithms.cpp
	@echo "Compiling..."
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

include/algo_info.hpp: src/algorithm/algorithms_info.json generate_header.py
	python3 generate_header.py $< $@

docs:
	doxygen

clean:
	rm -rf $(BUILD) pathvizgl
	rm -rf docs
	rm include/algo_info.hpp

all: include/algo_info.hpp pathvizgl docs