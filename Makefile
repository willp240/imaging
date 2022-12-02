§CXX=g++ -std=c++11
ROOT_FLAGS=`root-config --cflags --glibs`
CXX_OPTIONS= -O2 -std=c++11 -pedantic -Wall -Wextra
RAT_ROOT = $(RATROOT)
RAT_INC=$(RAT_ROOT)/include
RAT_EXTRN_INC=$(RAT_INC)/external
RAT_LIB_DIR=$(RAT_ROOT)/lib
RAT_LIB_NAME=RATEvent_Linux

SRC_FILES := $(wildcard *.cc )
OBJ_FILES := $(addprefix build/, $(notdir $(SRC_FILES:.cc=.o)))

PREFIX ?= /usr/local/bin

LIB_DIR=lib
LIB_NAME=img

LIB=$(LIB_DIR)/lib$(LIB_NAME).a

all: bin/imaging bin/imaging_alg2 bin/mcmc

bin/imaging: imaging.cc $(LIB)
	mkdir -p bin
	$(CXX) imaging.cc -I$(RAT_EXTRN_INC) -I$(RAT_INC) -w -L$(LIB_DIR) -L$(RAT_LIB_DIR) -l$(LIB_NAME) -l$(RAT_LIB_NAME) $(ROOT_FLAGS) -o $@

bin/imaging_alg2: imaging_alg2.cc $(LIB)
	mkdir -p bin
	$(CXX) imaging_alg2.cc -I$(RAT_EXTRN_INC) -I$(RAT_INC) -w -L$(LIB_DIR) -L$(RAT_LIB_DIR) -l$(LIB_NAME) -l$(RAT_LIB_NAME) $(ROOT_FLAGS) -o $@

bin/mcmc: mcmc.cc $(LIB)
	mkdir -p bin
	$(CXX) mcmc.cc -I$(RAT_EXTRN_INC) -I$(RAT_INC) -w -L$(LIB_DIR) -L$(RAT_LIB_DIR) -l$(LIB_NAME) -l$(RAT_LIB_NAME) $(ROOT_FLAGS) -o $@

build/%.o : %.cc
	mkdir -p build
	$(CXX) -c -w $< -I$(RAT_EXTRN_INC) -I$(RAT_INC) -w $(ROOT_FLAGS) -o $@

$(LIB) : $(OBJ_FILES)
	mkdir -p $(LIB_DIR)
	ar rcs  $@ $^

clean:
	rm -f bin/imaging
	rm -f bin/imaging_alg2
	rm -f bin/mcmc
	rm -f lib/*
