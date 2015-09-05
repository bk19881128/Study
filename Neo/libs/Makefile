include ../../Makefile.inc

TARGET = main

DIRS_TARGET =

all:$(TARGET) dir_build obj_clean

allgen:$(TARGET) dir_build_allgen

.PHONY : obj_clean clean
obj_clean :
	@$(RM) *.o
clean :
	@$(RM) $(TARGET) *.o
	@ for dir in ${DIRS_TARGET}; do (cd $${dir}; ${MAKE} clean) ; done

dir_build :
	@ for dir in ${DIRS_TARGET}; do (cd $${dir}; ${MAKE}) ; done

dir_build_allgen :
	@ for dir in ${DIRS_TARGET}; do (cd $${dir}; ${MAKE} allgen) ; done

main : main.cc model.cc neobaselib.cc neodebug.cc neolock.cc neolog.cc neomemmanager.cc neoqueue.cc neosafefunc.cc neothread.cc
	$(CXX) -o $@ $^ ${CXXFLAGS}

