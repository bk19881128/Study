include ../../../Makefile.inc

TARGET = 2_replace_string

DIRS_TARGET = 

all:$(TARGET) dir_build obj_clean

allgen:$(TARGET) dir_build_allgen

.PHONY : obj_clean clean
obj_clean :
	$(RM) *.o
clean :
	$(RM) $(TARGET) *.o
	@ for dir in ${DIRS_TARGET}; do (cd $${dir}; ${MAKE} clean) ; done

dir_build :
	@ for dir in ${DIRS_TARGET}; do (cd $${dir}; ${MAKE}) ; done

dir_build_allgen :
	@ for dir in ${DIRS_TARGET}; do (cd $${dir}; ${MAKE} allgen) ; done

replace_string : replace_string.cc
	$(CXX) -o $@ $^ ${CXXFLAGS}

