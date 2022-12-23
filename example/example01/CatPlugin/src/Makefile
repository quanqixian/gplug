include ./Rules.make

ROOT_PATH = ../../../..
CPPFLAGS = -I. \
           -I$(ROOT_PATH)/include \
		   -I$(ROOT_PATH)/include/interface/

BIN_DIR = ../bin
LIBS_DIR = $(ROOT_PATH)/lib/ 
CPPFLAGS += -D_REENTRANT
LDFLAGS += -L$(LIBS_DIR) -Wl,-rpath='$$ORIGIN:$$ORIGIN/$(ROOT_PATH)/lib/'


#Target file
TARGETSO = libCatPlugin.so

#Source and obj file
SRCS = $(wildcard *.c) $(wildcard *.cc) $(wildcard *.cpp) $(wildcard *.cxx)
OBJS = $(addsuffix .o, $(basename $(SRCS)))


#Gernation the target file
all: $(TARGETSO)

$(TARGETSO): $(OBJS)
	$(CXX) $(OPTS) $(LDFLAGS) -o $@ $^ 
	mkdir -p $(BIN_DIR) && cp $@  $(BIN_DIR)

%.o : %.c
	$(CC) -c $(OPTS) $(CPPFLAGS) $(CFLAGS) $< -o $@

%.o : %.cc
	$(CXX) -c $(OPTS) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

%.o : %.cpp
	$(CXX) -c $(OPTS) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

%.o : %.cxx
	$(CXX) -c $(OPTS) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

clean:
	-rm -f $(OBJS) $(TARGETSO)
	-rm -f *~
	-rm -f $(BIN_DIR)/$(TARGETSO)
