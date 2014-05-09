################################################################################
# NAMES
################################################################################

EXECUTABLE_RELEASE = littletsp
EXECUTABLE_DEBUG = littletspd
DEPS_DIR = build/dep
DEPS_FLAG = -MM
INCLUDE_DIR = .
INCLUDE_EXT = hpp
OBJECTS_RELEASE_DIR = build/release
OBJECTS_DEBUG_DIR = build/debug
SRC_DIR = .
SRC_EXT = cpp

################################################################################
# COMPILER AND COMPILER FLAGS
################################################################################

CPPFLAGS = -I$(INCLUDE_DIR)
CXX = g++
CXXFLAGS = -Wall -Werror -std=c++11
CXXFLAGS_DEBUG = -g -O0
CXXFLAGS_RELEASE = -O3
LDFLAGS = 

################################################################################
# TARGETS
################################################################################

all: release

release: $(shell mkdir -p $(OBJECTS_RELEASE_DIR))
release: CXXFLAGS += $(CXXFLAGS_RELEASE)
release: OBJS_DIR := $(OBJECTS_RELEASE_DIR)
release: $(EXECUTABLE_RELEASE)

debug: $(shell mkdir -p $(OBJECTS_DEBUG_DIR))
debug: CXXFLAGS += $(CXXFLAGS_DEBUG)
debug: OBJS_DIR := $(OBJECTS_DEBUG_DIR)
debug: $(EXECUTABLE_DEBUG)

print-%:
	@echo '$*=$($*)'

clean:
	rm -rf $(EXECUTABLE_RELEASE) $(EXECUTABLE_DEBUG) $(DEPS_DIR)/*.d \
		$(OBJECTS_DEBUG_DIR)/*.o $(OBJECTS_RELEASE_DIR)/*.o

.PHONY: release debug clean

################################################################################
# LISTS
################################################################################

DEPS := $(patsubst %.$(SRC_EXT), $(DEPS_DIR)/%.d, \
	$(notdir $(wildcard $(SRC_DIR)/*.$(SRC_EXT))))

SRCS := $(wildcard $(SRC_DIR)/*.$(SRC_EXT))

OBJECTS_DEBUG := $(patsubst $(SRC_DIR)/%.$(SRC_EXT), \
	$(OBJECTS_DEBUG_DIR)/%.o, $(SRCS))
OBJECTS_RELEASE := $(patsubst $(SRC_DIR)/%.$(SRC_EXT), \
	$(OBJECTS_RELEASE_DIR)/%.o, $(SRCS))

INCLUDES := $(wildcard $(INCLUDE_DIR)/*$(INCLUDE_EXT))

################################################################################
# OBJECTS AND DEPS 
################################################################################

# include all the dependency rules that exist
-include $(DEPS)

# compile objects and generate deps for them
$(OBJECTS_RELEASE_DIR)/%.o $(OBJECTS_DEBUG_DIR)/%.o : $(SRC_DIR)/%.$(SRC_EXT)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<
	@mkdir -p $(DEPS_DIR)
	@$(CXX) $(DEPS_FLAG) $(CPPFLAGS) $(CXXFLAGS) $< > $(*F).d.tmp
	@sed -e 's|.*\.o:|$@:|' < $(*F).d.tmp > $(DEPS_DIR)/$(*F).d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
		  sed -e 's/^ *//' -e 's/$$/:/' >> $(DEPS_DIR)/$(*F).d
	@rm -f $(*F).d.tmp

################################################################################
# BINARIES
################################################################################

# different executables for different targets
$(EXECUTABLE_DEBUG): $(OBJECTS_DEBUG)
	@echo "Building debug executable"
	$(CXX) $(CXXFLAGS) $(CXX_EXTRAS) $^ $(LDFLAGS) -o $@

$(EXECUTABLE_RELEASE): $(OBJECTS_RELEASE)
	$(CXX) $(CXXFLAGS) $(CXX_EXTRAS) $^ $(LDFLAGS) -o $@
