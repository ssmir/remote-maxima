TOP := ..
include $(TOP)/rules.common

CPPFLAGS += -I$(CXXTEST_HOME)
LOCAL_OBJ := MaximaInstanceTest.o

test: alltests libSimpleService.$(SOEXT)
	./alltests

alltests : MaximaInstanceTest.o TestRunnerTest.o $(TOP)/lib/libMaximaAPI.a
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

%Test.cc : %Test.h
	perl $(CXXTEST_HOME)/cxxtestgen.pl --part --have-eh --abort-on-fail -o $@ $<

TestRunnerTest.cc :
	perl $(CXXTEST_HOME)/cxxtestgen.pl --error-printer --root --have-eh \
		--abort-on-fail -o $@

MaximaInstanceTest.o TestRunnerTest.o : %.o : %.cc
	$(CXX) -c $(CXXFLAGS) $(CFLAGS) $(CPPFLAGS) -o $@ $<

libSimpleService.$(SOEXT) : SimpleService.o $(TOP)/lib/libIARnet.$(SOEXT)
	$(CXX) $(SOFLAGS) $(LDFLAGS) -o $@ $^ -lIARnet $(LIBS) $(ICE_LIBS)

clean:
	rm -f *.o *.d alltests *Test.cc libSimpleService.$(SOEXT)

include $(TOP)/rules.deps

