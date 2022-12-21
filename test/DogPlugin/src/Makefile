include ./Rules.make

CPPFLAGS = -I. \
           -I../../../include \
		   -I../../../include/interface/

BIN_DIR = ../bin
LIBS_DIR = ../../../lib/ 
CPPFLAGS += -D_REENTRANT
LDFLAGS += -L$(LIBS_DIR) -Wl,-rpath='$$ORIGIN:$$ORIGIN/../../../lib/'


#Link the .so or .a library
LIBS =  

#Target file
TARGETSO = libDogPlugin.so

#Source and obj file
SRCS = $(wildcard *.c) $(wildcard *.cc) $(wildcard *.cpp) $(wildcard *.cxx)
OBJS = $(addsuffix .o, $(basename $(SRCS)))


#Gernation the target file
all: $(TARGETSO)

$(TARGETSO): $(OBJS)
	$(CXX) $(OPTS) $(LDFLAGS) -o $@ $^ $(LIBS)
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
