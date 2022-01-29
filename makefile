SRC_DIR := src
INC_DIR := include
OBJ_DIR := build/obj
BIN_DIR := build/bin

CXX := g++
INCFLAGS := -I$(INC_DIR)
CXXFLAGS := -pthread -lsockpp -lisal

ALLFILES := $(OBJ_DIR)/connection_solver.o
ALLFILES += $(OBJ_DIR)/socket_solver.o
ALLFILES += $(OBJ_DIR)/local_file_solver.o

ALLFILES += $(OBJ_DIR)/config_reader.o
ALLFILES += $(OBJ_DIR)/address_handler.o

ALLFILES += $(OBJ_DIR)/node_connector.o
ALLFILES += $(OBJ_DIR)/memory_pool.o

ALLFILES += $(OBJ_DIR)/calculator.o
ALLFILES += $(OBJ_DIR)/compute_tool.o

ALLFILES += $(OBJ_DIR)/bandwidth_info.o
ALLFILES += $(OBJ_DIR)/tree_builder.o

ALLFILES += $(OBJ_DIR)/node_repairer.o
ALLFILES += $(OBJ_DIR)/master_node.o
ALLFILES += $(OBJ_DIR)/task_getter.o


all : $(BIN_DIR)/node_main $(BIN_DIR)/master_main

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cc
	$(CXX) $(INCFLAGS) -c $< -o $@ -O3

$(BIN_DIR)/%_main : $(OBJ_DIR)/%_main.o $(ALLFILES)
	$(CXX) $^ -o $@ $(CXXFLAGS) -O3

.PHONY: clean
clean:
	-rm build/obj/*.o build/bin/* -f
