ifeq ($(CONFIG),)
	CONFIG = release_x64
endif

ifeq ($(VERBOSE),)
	VERBOSE=1
endif


all : bin/itoa_release_x64_gmake
	cd bin && ./itoa_release_x64_gmake $(ARGS)
	cd result && make -f makefile

bin/itoa_%_gmake : build/gmake/itoa.make
	cd build/gmake && make -f itoa.make config=$(CONFIG) verbose=$(VERBOSE)

clean : 
	rm -rf build/gmake
	rm -rf build/vs2005
	rm -rf build/vs2008
	rm -rf build/vs2010
	rm -rf intermediate
	rm -rf src/machine.h
	rm -rf bin
	cd result && make -f makefile clean

setup :
	cd build && ./premake.sh && ./machine.sh
	
	
build/gmake/itoa.make : setup	
	
clean_status :
	@echo "Filesystem status according to GIT"
	@git clean -dfxn
