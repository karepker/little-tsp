################################################################################
# COMPILER AND COMPILER FLAGS
################################################################################

CXX=g++
CXXFLAGS=-Wall -Werror -std=c++11
CXXFLAGSD=-g
CXXFLAGSR=-O3

################################################################################
# DIRECTORIES
################################################################################

SRCDIR = src
BINDIR = bin
BINDDIR = bind
INCLUDEDIR = include

################################################################################
# OBJECTS AND BINARIES
################################################################################

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard ${SRCDIR}/*.cpp))
EXECUTABLE = littletsp
EXECUTABLED = littletspd

################################################################################
# TARGETS
################################################################################

all: release

release: CXXFLAGS += $(CXXFLAGSR)
release: $(EXECUTABLE)

debug: CXXFLAGS += $(CXXFLAGSD)
debug: $(EXECUTABLED)

clean:
	rm -rf $(EXECUTABLE) $(EXECUTABLED) *.o

################################################################################
# BINARIES
################################################################################

$(EXECUTABLE): $(RELEASE_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

$(EXECUTABLED): $(DEBUG_OBJECTS) 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

################################################################################
# OBJECTS
################################################################################

main.o: main.cpp interaction.hpp graph.hpp common.hpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

basicgraph.o: basicgraph.cpp basicgraph.hpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

graph.o: graph.cpp graph.hpp matrix.hpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

opttsp.o: opttsp.cpp graph.hpp pathinfo.hpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

naivetsp.o: naivetsp.cpp graph.hpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

pathinfo.o: pathinfo.cpp pathinfo.hpp basicgraph.hpp
	$(CXX) -c $(CXXFLAGS) $< -o $@
