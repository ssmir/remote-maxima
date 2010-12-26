TOP := ..
include $(TOP)/rules.common

JAVA_SOURCES := $(shell find . -name *.java)
JNI_HEADERS := $(TOP)/src/ru_isa_dcs_ssmir_maxima_MaximaInstance.h

default: $(TOP)/lib/MaximaInstance.jar $(JNI_HEADERS)

$(TOP)/lib/MaximaInstance.jar : javaClasses
	$(JAVA_HOME)/bin/jar vcf $@ `find ru -name '*.class'`

$(JNI_HEADERS) : %.h : javaClasses
	$(JAVA_HOME)/bin/javah -o $@ $(subst _,.,$(notdir $*))

javaClasses : $(JAVA_SOURCES)
	$(JAVA_HOME)/bin/javac $?
	touch $@

clean:
	rm -f $(JNI_HEADERS) $(shell find . -name *.class) javaClasses \
		$(TOP)/lib/MaximaInstance.jar

