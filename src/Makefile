TOP := ..
include $(TOP)/rules.common

LIBS += $(ICE_LIBS)

NODE_OBJ = ServantLocator.o Security.o AccessDeniedServant.o Node.o main.o \
	InfoService.o Service.o ServiceContext.o
MAXIMA_SERVICE_OBJ = Maxima.o MaximaFactoryI.o MaximaResourceI.o FileServerImpl.o \
	MasterFactoryI.o FileServer.o
LOCAL_OBJ := $(NODE_OBJ) $(MAXIMA_SERVICE_OBJ) MaximaInstanceTest.o MaximaInstance.o

all: $(TOP)/bin/node $(TOP)/lib/libMaximaService.$(SOEXT) $(TOP)/lib/libMaximaInstance.$(JNI_SUFFIX)

$(TOP)/bin/node: main.o $(TOP)/lib/libIARnet.$(SOEXT)
	$(CXX) $(LDFLAGS) -o $@ $< -lIARnet $(LIBS)

$(TOP)/lib/libIARnet.$(SOEXT) : ServantLocator.o Security.o AccessDeniedServant.o \
	Node.o InfoService.o Service.o ServiceContext.o
	$(CXX) $(SOFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(NODE_OBJ) : %.o : %.cc
	$(CXX) -c $(CXXFLAGS) -DNODEDLL $(CFLAGS) $(CPPFLAGS) -o $@ $<

$(TOP)/lib/libMaximaService.$(SOEXT): $(MAXIMA_SERVICE_OBJ) $(TOP)/lib/libMaximaAPI.a \
		$(TOP)/lib/libIARnet.$(SOEXT)
	$(CXX) $(SOFLAGS) $(LDFLAGS) -o $@ \
		$(filter-out $(TOP)/lib/libIARnet.$(SOEXT),$^) -lIARnet $(LIBS) 

$(TOP)/lib/libMaximaAPI.a : MaximaInstance.o
	ar -r $@ $?

$(TOP)/lib/libMaximaInstance.$(JNI_SUFFIX) : ru_isa_dcs_ssmir_maxima_MaximaInstance.o $(TOP)/lib/libMaximaAPI.a
	$(CXX) $(SOFLAGS) $(LDFLAGS) -o $@ $^ $(call BOOST_WRAP_SHARED_LIBS, boost_regex boost_filesystem boost_system)

clean:
	rm -f *.o *.d $(TOP)/bin/node $(TOP)/lib/libMaximaAPI.a \
		$(TOP)/lib/libIARnet.$(SOEXT) $(TOP)/lib/libMaximaService.$(SOEXT) \
		$(TOP)/lib/libMaximaInstance.$(JNI_SUFFIX)

include $(TOP)/rules.deps

