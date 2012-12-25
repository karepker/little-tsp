################################################################################
# DIRECTORIES
################################################################################

SRC_DIR = src
OBJSR_DIR = build/release
OBJSD_DIR = build/debug
INCLUDE_DIR = include

################################################################################
# COMPILER AND COMPILER FLAGS
################################################################################

CXX = g++
CXXFLAGS = -Wall -Werror -std=c++11
CXXFLAGSD = -g
CPPFLAGS = -I$(INCLUDE_DIR)
OBJS_DIR =
CXXFLAGSR = -O3

################################################################################
# OBJECTS AND BINARIES
################################################################################

OBJECTS = main.o basicgraph.o graph.o opttsp.o naivetsp.o pathinfo.o
EXECUTABLE = littletsp
EXECUTABLED = littletspd

################################################################################
# TARGETS
################################################################################

all: release

release: CXXFLAGS += $(CXXFLAGSR)
release: OBJS_DIR += $(OBJSR_DIR)
release: $(EXECUTABLE)

debug: CXXFLAGS += $(CXXFLAGSD)
debug: OBJS_DIR += $(OBJSD_DIR)
debug: $(EXECUTABLED)

clean:
	rm -rf $(EXECUTABLE) $(EXECUTABLED) *.o

################################################################################
# BINARIES
################################################################################

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

$(EXECUTABLED): $(OBJECTS) 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

################################################################################
# OBJECTS
################################################################################

main.o: $(SRC_DIR)/main.cpp $(INCLUDE_DIR)/interaction.hpp \
	$(INCLUDE_DIR)/graph.hpp $(INCLUDE_DIR)/common.hpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $(OBJS_DIR)/$@

basicgraph.o: $(SRC_DIR)/basicgraph.cpp $(INCLUDE_DIR)/basicgraph.hpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $(OBJS_DIR)/$@

graph.o: $(SRC_DIR)/graph.cpp $(INCLUDE_DIR)/graph.hpp \
	$(INCLUDE_DIR)/matrix.hpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $(OBJS_DIR)/$@

opttsp.o: $(SRC_DIR)/opttsp.cpp $(INCLUDE_DIR)/graph.hpp \
	$(INCLUDE_DIR)/pathinfo.hpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $(OBJS_DIR)/$@

naivetsp.o: $(SRC_DIR)/naivetsp.cpp $(INCLUDE_DIR)/graph.hpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $(OBJS_DIR)/$@

pathinfo.o: $(SRC_DIR)/pathinfo.cpp $(INCLUDE_DIR)/pathinfo.hpp \
	$(INCLUDE_DIR)/basicgraph.hpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $(OBJS_DIR)/$@
