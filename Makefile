CXX=g++ -std=c++11
ROOT_FLAGS=`root-config --cflags --glibs`
CXX_OPTIONS= -O2 -std=c++11 -pedantic -Wall -Wextra
RAT_ROOT = $(RATROOT)
RAT_INC=$(RAT_ROOT)/include
RAT_EXTRN_INC=$(RAT_INC)/external
RAT_LIB_DIR=$(RAT_ROOT)/lib
RAT_LIB_NAME=RATEvent_Linux

SRC_FILES := $(imaging.cc )
OBJ_FILES := $(addprefix build/, $(notdir $(SRC_FILES:.cc=.o)))

PREFIX ?= /usr/local/bin

LIB_DIR=lib
LIB_NAME=imaging

LIB=$(LIB_DIR)/lib$(LIB_NAME).a

all: bin/imaging

bin/imaging: imaging.cc $(LIB)
	mkdir -p bin
	$(CXX) imaging.cc -I$(RAT_EXTRN_INC) -I$(RAT_INC) -w -L$(LIB_DIR) -L$(RAT_LIB_DIR) -l$(LIB_NAME) -l$(RAT_LIB_NAME) $(ROOT_FLAGS) -o $@

$(LIB) : $(OBJ_FILES)
	mkdir -p $(LIB_DIR)
	ar rcs  $@ $^

clean:
	rm -f bin/imaging
	rm -f lib/*
