TOP := .
include rules.common

all: create_dirs slice_build src_build examples_build test_build

examples_build : src_build
test_build : src_build
src_build : slice_build java_build create_dirs

create_dirs :
	mkdir -p lib

%_build :
	$(MAKE) -C $*

%_clean :
	$(MAKE) -C $* clean

clean: slice_clean src_clean test_clean examples_clean java_clean

