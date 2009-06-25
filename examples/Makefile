TOP := ..
include $(TOP)/rules.common

CPPFLAGS += -I$(ICE_HOME)/include -I$(TOP)/src
LDFLAGS += -L$(ICE_HOME)/lib
LIBS += $(ICE_LIBS)

LOCAL_OBJ := miclient.o mclient.o client.o

all: mclient miclient client
	echo MaximaMasterFactory.MaximaPath = $(MAXIMA_APP) > maxima_path.conf
	echo 'export PATH=../bin:$$PATH' > paths.sh
	echo "export ICE_HOME="$(ICE_HOME) >> paths.sh

mclient: mclient.o $(TOP)/lib/libMaximaAPI.a
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

miclient: miclient.o $(TOP)/lib/libMaximaAPI.a
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

client: client.o $(TOP)/src/Maxima.o $(TOP)/src/FileServer.o $(TOP)/src/FileServerImpl.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f *.o *.d mclient miclient client maxima_path.conf paths.sh

include $(TOP)/rules.deps

