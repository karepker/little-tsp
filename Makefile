################################################################################
# NAMES
################################################################################

SRC_DIR = src
OBJSR_DIR = build/release
OBJSD_DIR = build/debug
INCLUDE_DIR = include
EXECUTABLER = littletsp
EXECUTABLED = littletspd

################################################################################
# COMPILER AND COMPILER FLAGS
################################################################################

CXX = g++
CXXFLAGS = -Wall -Werror -std=c++11
CXXFLAGSD = -g
CXXFLAGSR = -O3
CPPFLAGS = -I$(INCLUDE_DIR)

################################################################################
# TARGETS
################################################################################

all: release

release: CXXFLAGS += $(CXXFLAGSR)
release: OBJS_DIR := $(OBJSR_DIR)
release: $(EXECUTABLER)

debug: CXXFLAGS += $(CXXFLAGSD)
debug: OBJS_DIR := $(OBJSD_DIR)
debug: $(EXECUTABLED)

clean:
	rm -rf $(EXECUTABLER) $(EXECUTABLED) $(OBJSD_DIR)/*.o $(OBJSR_DIR)/*.o

################################################################################
# OBJECTS AND BINARIES
################################################################################

# different objects
OBJECTSD = $(patsubst $(SRC_DIR)/%.cpp, $(OBJSD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cpp))
OBJECTSR = $(patsubst $(SRC_DIR)/%.cpp, $(OBJSR_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cpp))

INCLUDES = $(wildcard include/*.hpp)

# different executables for different targets
$(EXECUTABLED): $(OBJECTSD)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

$(EXECUTABLER): $(OBJECTSR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

# individual object rules
$(OBJSD_DIR)/%.o $(OBJSR_DIR)/%.o: $(SRC_DIR)/%.cpp $(INCLUDES) 	
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@
