TOP := ..
include $(TOP)/rules.common
SLICE = LD_LIBRARY_PATH=$(ICE_HOME)/lib $(ICE_HOME)/bin/slice2cpp --source-ext=cc -I.

GENERATED = $(TOP)/include/IARnet/InfoService.h $(TOP)/src/InfoService.cc \
	$(TOP)/include/IARnet/Security.h $(TOP)/src/Security.cc \
	$(TOP)/src/Maxima.h $(TOP)/src/Maxima.cc $(TOP)/src/FileServer.h \
	$(TOP)/src/FileServer.cc

all: $(GENERATED)

$(TOP)/include/IARnet/InfoService.h $(TOP)/src/InfoService.cc : InfoService.ice
	$(SLICE) --include-dir IARnet $<
	mv InfoService.cc $(TOP)/src/InfoService.cc
	mv InfoService.h $(TOP)/include/IARnet/InfoService.h

$(TOP)/include/IARnet/Security.h $(TOP)/src/Security.cc : Security.ice
	$(SLICE) --include-dir IARnet -I$(ICE_HOME_SLICE) $<
	mv Security.cc $(TOP)/src/Security.cc
	mv Security.h $(TOP)/include/IARnet/Security.h

$(TOP)/src/Maxima.h $(TOP)/src/Maxima.cc : Maxima.ice
	$(SLICE) --output-dir $(TOP)/src $<
	
$(TOP)/src/FileServer.h $(TOP)/src/FileServer.cc : FileServer.ice
	$(SLICE) --output-dir $(TOP)/src $<

clean: 
	rm -f $(GENERATED)

