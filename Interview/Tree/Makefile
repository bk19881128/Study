include ../../Makefile.inc

TARGET = 1_problem 2_problem

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

1_problem : 1_problem.cc tree_common.cc  ../../Utility/common.cc
	$(CXX) -o $@ $^ ${CXXFLAGS}

2_problem : 2_problem.cc tree_common.cc ../../Utility/common.cc
	$(CXX) -o $@ $^ ${CXXFLAGS}

