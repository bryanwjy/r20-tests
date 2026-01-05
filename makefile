
MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MKFILE_DIR := $(patsubst %/,%,$(dir $(MKFILE_PATH)))

CXX ?= c++
RXX_SRC ?= $(dir $(MKFILE_DIR))r20
OUTPUT_DIR ?= $(RXX_SRC)/build

INTERMEDIATE_DIR := $(OUTPUT_DIR)/obj
TEST_ROOT := $(MKFILE_DIR)
GCC_DIR := $(TEST_ROOT)/gcc
LLVM_DIR := $(TEST_ROOT)/llvm
RXX_DIR := $(TEST_ROOT)/rxx

TEST_SRCS := $(shell find $(GCC_DIR) $(LLVM_DIR) $(RXX_DIR) -name '*.pass.cpp')
TEST_SUBDIRS := $(shell find $(GCC_DIR) $(LLVM_DIR) $(RXX_DIR) -type d)
CXX_FLAGS := $(CXX_FLAGS) -std=c++23 -O0 -g -I$(RXX_SRC) -ftemplate-backtrace-limit=0
LINKER_FLAGS := $(LINKER_FLAGS)

BUILD_SUBDIRS := $(patsubst $(TEST_ROOT)/%,%,$(TEST_SUBDIRS))
BUILD_OBJECTS := $(addsuffix .o, $(TEST_SRCS:$(TEST_ROOT)/%=%))
DEPENDENCIES := $(BUILD_OBJECTS:.o=.d)

PASS_OBJECTS := $(filter-out %.compile.pass.cpp.o,$(BUILD_OBJECTS))
PASS_EXES := $(patsubst %.cpp.o,%,$(PASS_OBJECTS))
COMPILE_OBJECTS := $(patsubst %.cpp.o,%,$(filter %.compile.pass.cpp.o,$(BUILD_OBJECTS)))
PREPROCESS_OBJECTS := $(addsuffix .i, $(TEST_SRCS:$(TEST_ROOT)/%=%))


define subdir_to_crc
$(patsubst $(TEST_ROOT)/%.cpp,$(OUTPUT_DIR)/%.crc,$(wildcard $(TEST_ROOT)/$(1)/*.pass.cpp)) \
$(patsubst $(TEST_ROOT)/%.cpp,$(OUTPUT_DIR)/%.crc,$(wildcard $(TEST_ROOT)/$(1)/**/*.pass.cpp))
endef

.PHONY: all clean run compile $(BUILD_SUBDIRS) $(PASS_EXES) $(BUILD_OBJECTS) $(PREPROCESS_OBJECTS)

all: run
	@

run: $(PASS_EXES) $(COMPILE_OBJECTS)
	@

compile: $(BUILD_OBJECTS)
	@

print:
	@echo $(BUILD_OBJECTS)

$(COMPILE_OBJECTS):%: $(INTERMEDIATE_DIR)/%.cpp.o makefile
	@

$(PASS_EXES):%: $(OUTPUT_DIR)/%.crc
	@

.SECONDEXPANSION:
$(BUILD_SUBDIRS):%: $$(call subdir_to_crc,%)
	@

$(OUTPUT_DIR)/%.compile.pass.crc: $(INTERMEDIATE_DIR)/%.compile.pass.cpp.o
	@

$(INTERMEDIATE_DIR)/%.compile.pass.cpp.o: $(TEST_ROOT)/%.compile.pass.cpp makefile
	@mkdir -p '$(@D)'
	@$(CXX) $(CXX_FLAGS) -MMD -MP -MF '$(@:.o=.d)' -MT '$@' -c $< -o '$@' && \
	echo "\033[0;34mCOMPILE\033[0m $(patsubst $(INTERMEDIATE_DIR)/%,%,$@): \033[0;32mSUCCESS\033[0m"  || \
	{ echo "\033[0;34mCOMPILE\033[0m $(patsubst $(INTERMEDIATE_DIR)/%,%,$@): \033[0;31mFAILED\033[0m"; exit 1; }

$(OUTPUT_DIR)/%.pass.crc: $(OUTPUT_DIR)/%.pass
	@$< && echo "\033[0;34mTEST\033[0m $(patsubst $(OUTPUT_DIR)/%,%,$<): \033[0;32mSUCCESS\033[0m" && \
	cksum $< > $@ || { echo "TEST $(OUTPUT_DIR)/%,%,$<): \033[0;31mFAILED\033[0m" && rm -f $@; exit 1; }

$(OUTPUT_DIR)/%.pass: $(INTERMEDIATE_DIR)/%.pass.cpp.o makefile
	@mkdir -p '$(@D)'
	@echo "Building test" $(patsubst $(OUTPUT_DIR)/%,%,$@)
	@$(CXX) $(LINKER_FLAGS) $< -o $@

$(BUILD_OBJECTS):%.cpp.o: $(INTERMEDIATE_DIR)/%.cpp.o
	@

$(INTERMEDIATE_DIR)/%.pass.cpp.o: $(TEST_ROOT)/%.pass.cpp makefile
	@mkdir -p '$(@D)'
	@$(CXX) $(CXX_FLAGS) -MMD -MP -MF '$(@:.o=.d)' -MT '$@' -c $< -o '$@'

$(PREPROCESS_OBJECTS):%.cpp.i: $(INTERMEDIATE_DIR)/%.cpp.i
	@

$(INTERMEDIATE_DIR)/%.pass.cpp.i: $(TEST_ROOT)/%.pass.cpp makefile
	@mkdir -p '$(@D)'
	@$(CXX) $(CXX_FLAGS) -MMD -MP -MF '$(@:.i=.d)' -MT '$@' -E $< -o '$@'

-include $(addprefix $(INTERMEDIATE_DIR)/,$(DEPENDENCIES))

clean:
	@find $(INTERMEDIATE_DIR) -name '*.i' -delete
	@find $(INTERMEDIATE_DIR) -name '*.o' -delete
	@find $(INTERMEDIATE_DIR) -name '*.d' -delete
	@find $(INTERMEDIATE_DIR) -name '*.prep.cpp' -delete
	@find $(OUTPUT_DIR) -name '*.crc' -delete
