# This makefile automatically grabs the files in $(SRC),\
	builds the dependencies which saved in $(DEP),\
	compiles the grabbed source files into object files in $(OBJ),\
	and finally links the object files and outputs:\
		main to $(BIN) and test main to $(TEST).

# To use this makefile needs:\
	named the source files with extension: .cc\
	the source main files' names end with: _main.cc\
	the source test files' names end with: _test.cc

# -- File Directories --
SRC := src
OBJ := obj
BIN := bin

con := -
DEP := $(OBJ)/dep
TEST := $(BIN)/test

# -- Compile Configurations --
CXX := g++
CXXFLAGS := -std=c++14 -I$(SRC) -Wall -O3
LDFLAGS := -lpthread -lsockpp -lisal -O3

# -- Personal File Type Change Functions --
cc_to_o = $(patsubst $(SRC)$(con)%.cc,$(OBJ)/%.o,\
		  $(subst /,$(con),$(1)))

o_to_cc = $(subst $(con),/,\
		  $(patsubst $(OBJ)%.o,$(SRC)%.cc,$(1)))

# -- File Lists --
CCFS := $(shell find $(SRC) -name "*.cc")
DEPS := $(CCFS:$(SRC)%.cc=$(DEP)%.d)

OBJS := $(call cc_to_o,$(CCFS))
MAINS := $(filter %_main,$(OBJS:$(OBJ)%.o=$(BIN)%))
TESTS := $(filter %_test,$(OBJS:$(OBJ)%.o=$(TEST)%))
SUPPORTS := $(filter-out %_main.o %_test.o,$(OBJS))

# -- Rules --
all: $(MAINS)
	@echo All targets exist:$(MAINS:%="\n\t"%)
	@echo Make Ended.

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPS)
endif

$(BIN)/%_main: $(OBJ)/%_main.o $(SUPPORTS)
	@echo "    "Linking file: $@
	@mkdir -p $(BIN)
	@$(CXX) $^ -o $@ $(LDFLAGS)

$(TEST)/%: $(OBJ)/%.o $(SUPPORTS)
	@echo "    "Linking Test file: $@
	@mkdir -p $(TEST)
	@$(CXX) $^ -o $@ $(LDFLAGS)

$(OBJ)/%.o:
	@echo --Compiling file: $@
	@mkdir -p $(OBJ)
	@$(CXX) $(CXXFLAGS) -c $(call o_to_cc,$@) -o $@

$(filter-out %_main.d %_test.d,$(DEPS)): $(DEP)/%.d: $(SRC)/%.hh
$(DEP)/%.d: $(SRC)/%.cc
	@echo Building dependency of file: $<
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -MM $< -MT $(call cc_to_o,$<) -MF $@

# -- Test Files --
.PHONY: test
test: $(TESTS)
	@echo All test files exist:$(TESTS:%="\n\t"%)
	@echo Make Ended.

# -- Clean Rule --
.PRECIOUS: $(OBJ)/*
.PHONY: clean
clean:
	@echo Removing directories: $(OBJ)/ and $(BIN)/...
	@rm -rf $(OBJ) $(BIN)
	@echo Finished cleaning.
