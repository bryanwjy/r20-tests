
MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MKFILE_DIR := $(patsubst %/,%,$(dir $(MKFILE_PATH)))

CXX ?= c++
RXX_PATH ?= $(dir $(MKFILE_DIR))r20
OUTPUT_DIR ?= $(RXX_PATH)/build

INTERMEDIATE_DIR := $(OUTPUT_DIR)/obj
TEST_ROOT := $(MKFILE_DIR)
GCC_DIR := $(TEST_ROOT)/gcc
LLVM_DIR := $(TEST_ROOT)/llvm

TEST_SRCS := $(shell find $(GCC_DIR) $(LLVM_DIR) -name '*.pass.cpp')
TEST_OBJECTS := $(addsuffix .o, $(TEST_SRCS:$(TEST_ROOT)/%=%))
CXX_FLAGS := -std=c++20 -O1 -I$(RXX_PATH)
LINKER_FLAGS := 

DEPENDENCIES := $(TEST_OBJECTS:.o=.d)

PASS_OBJECTS := $(filter-out %.compile.pass.cpp.o,$(TEST_OBJECTS))
PASS_EXES := $(patsubst %.cpp.o,%,$(PASS_OBJECTS))

COMPILE_OBJECTS := $(filter %.compile.pass.cpp.o,$(TEST_OBJECTS))

.PHONY = all clean run compile $(TEST_OBJECTS)

all: run
	@

run: $(PASS_EXES) $(COMPILE_OBJECTS)
	@

compile: $(TEST_OBJECTS)
	@

print:
	@echo $(TEST_OBJECTS)

$(PASS_EXES):%: $(OUTPUT_DIR)/%.crc
	@

$(OUTPUT_DIR)/%.crc: $(OUTPUT_DIR)/%
	@$< && echo "\033[0;34mTEST\033[0m $(@:.crc=): \033[0;32mSUCCESS\033[0m" && \
	cksum $< > $<.crc || \
	{ echo "Test $@: \033[0;31mFAILED\033[0m" && rm -f $<.crc; exit 1; }

$(OUTPUT_DIR)/%.pass: $(INTERMEDIATE_DIR)/%.pass.cpp.o makefile
	@mkdir -p '$(@D)'
	@echo "Building test" $(patsubst $(OUTPUT_DIR)/%,%,$@)
	@$(CXX) $(LINKER_FLAGS) $< -o $@

$(TEST_OBJECTS):%.cpp.o: $(INTERMEDIATE_DIR)/%.cpp.o
	@

$(INTERMEDIATE_DIR)/%.compile.pass.cpp.o: $(TEST_ROOT)/%.compile.pass.cpp makefile
	@mkdir -p '$(@D)'
	@$(CXX) $(CXX_FLAGS) -MMD -MP -MF '$(@:.o=.d)' -MT '$@' -c $< -o '$@' && \
	echo "\033[0;34mCOMPILE\033[0m $(patsubst $(INTERMEDIATE_DIR)/%,%,$@): \033[0;32mSUCCESS\033[0m"  || \
	{ echo "\033[0;34mCOMPILE\033[0m $(patsubst $(INTERMEDIATE_DIR)/%,%,$@): \033[0;31mFAILED\033[0m"; exit 1; }

$(INTERMEDIATE_DIR)/%.pass.cpp.o: $(TEST_ROOT)/%.pass.cpp makefile
	@mkdir -p '$(@D)'
	@$(CXX) $(CXX_FLAGS) -MMD -MP -MF '$(@:.o=.d)' -MT '$@' -c $< -o '$@'

-include $(addprefix $(INTERMEDIATE_DIR)/,$(DEPENDENCIES))

clean:
	@rm -rf $(INTERMEDIATE_DIR)/**/*.o $(INTERMEDIATE_DIR)/**/*.d $(INTERMEDIATE_DIR)/**/*.prep.cpp $(OUTPUT_DIR)/
