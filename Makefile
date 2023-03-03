CXX=g++ -std=c++11
ROOT_FLAGS=`root-config --cflags --glibs`
CXX_OPTIONS= -O2 -std=c++11 -pedantic -Wall -Wextra
RAT_ROOT = $(RATROOT)
RAT_INC=$(RAT_ROOT)/include
RAT_EXTRN_INC=$(RAT_INC)/external
RAT_LIB_DIR=$(RAT_ROOT)/lib
RAT_LIB_NAME=RATEvent_Linux

SRC_FILES := $(wildcard src/*.cc )
OBJ_FILES := $(addprefix build/, $(notdir $(SRC_FILES:.cc=.o)))
INC_DIR=./src

PREFIX ?= /usr/local/bin

LIB_DIR=lib
LIB_NAME=img

LIB=$(LIB_DIR)/lib$(LIB_NAME).a

all: bin/adaptive_grid bin/adaptive_grid_noreducet bin/adaptive_grid_noreducepmt bin/adaptive_grid_pmtsel bin/adaptive_grid_tsel bin/adaptive_grid_tselb

bin/adaptive_grid: adaptive_grid.cc $(LIB)
	mkdir -p bin
	$(CXX) adaptive_grid.cc -I./ -I$(SRC_FILES) -I$(RAT_EXTRN_INC) -I$(RAT_INC) -I$(INC_DIR) -w -L$(LIB_DIR) -L$(RAT_LIB_DIR) -l$(LIB_NAME) -l$(RAT_LIB_NAME) $(ROOT_FLAGS) -o $@

bin/adaptive_grid_noreducet: adaptive_grid_noreducet.cc $(LIB)
	mkdir -p bin
	$(CXX) adaptive_grid_noreducet.cc -I./ -I$(SRC_FILES) -I$(RAT_EXTRN_INC) -I$(RAT_INC) -I$(INC_DIR) -w -L$(LIB_DIR) -L$(RAT_LIB_DIR) -l$(LIB_NAME) -l$(RAT_LIB_NAME) $(ROOT_FLAGS) -o $@

bin/adaptive_grid_noreducepmt: adaptive_grid_noreducepmt.cc $(LIB)
	mkdir -p bin
	$(CXX) adaptive_grid_noreducepmt.cc -I./ -I$(SRC_FILES) -I$(RAT_EXTRN_INC) -I$(RAT_INC) -I$(INC_DIR) -w -L$(LIB_DIR) -L$(RAT_LIB_DIR) -l$(LIB_NAME) -l$(RAT_LIB_NAME) $(ROOT_FLAGS) -o $@

bin/adaptive_grid_pmtsel: adaptive_grid_pmtsel.cc $(LIB)
	mkdir -p bin
	$(CXX) adaptive_grid_pmtsel.cc -I./ -I$(SRC_FILES) -I$(RAT_EXTRN_INC) -I$(RAT_INC) -I$(INC_DIR) -w -L$(LIB_DIR) -L$(RAT_LIB_DIR) -l$(LIB_NAME) -l$(RAT_LIB_NAME) $(ROOT_FLAGS) -o $@

bin/adaptive_grid_tsel: adaptive_grid_tsel.cc $(LIB)
	mkdir -p bin
	$(CXX) adaptive_grid_tsel.cc -I./ -I$(SRC_FILES) -I$(RAT_EXTRN_INC) -I$(RAT_INC) -I$(INC_DIR) -w -L$(LIB_DIR) -L$(RAT_LIB_DIR) -l$(LIB_NAME) -l$(RAT_LIB_NAME) $(ROOT_FLAGS) -o $@

bin/adaptive_grid_tselb: adaptive_grid_tselb.cc $(LIB)
		mkdir -p bin
		$(CXX) adaptive_grid_tselb.cc -I./ -I$(SRC_FILES) -I$(RAT_EXTRN_INC) -I$(RAT_INC) -I$(INC_DIR) -w -L$(LIB_DIR) -L$(RAT_LIB_DIR) -l$(LIB_NAME) -l$(RAT_LIB_NAME) $(ROOT_FLAGS) -o $@

$(LIB) : $(OBJ_FILES)
	mkdir -p $(LIB_DIR)
	ar rcs  $@ $^

build/%.o : src/%.cc
	mkdir -p build
	$(CXX) -c -w $< -I$(INC_DIR) -I$(RAT_EXTRN_INC) -I$(RAT_INC) -w $(ROOT_FLAGS) -o $@

clean:
	rm -f build/*
	rm -f bin/adaptive_grid
	rm -f bin/adaptive_grid_noreducet
	rm -f bin/adaptive_grid_noreducepmt
	rm -f bin/adaptive_grid_pmtsel
	rm -f bin/adaptive_grid_tsel
	rm -f bin/adaptive_grid_tselb
	rm -f lib/*
