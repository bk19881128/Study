include ../../Makefile.inc

TARGET = capacity erase1 erase2 find_ops \
		find-str refreshIter stack \
		str_assignOps substr useConvs

DIRS_TARGET = 4 11 14 15 18 24 26 27 31 41 43 44 45 47 50 52

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
    

capacity : capacity.cc
	$(CXX) -o $@ $^ ${CXXFLAGS}


erase1 : erase1.cc
	$(CXX) -o $@ $^ ${CXXFLAGS}

erase2 : erase2.cc
	$(CXX) -o $@ $^ ${CXXFLAGS}

find_ops : find_ops.cc
	$(CXX) -o $@ $^ ${CXXFLAGS}

find-str : find-str.cc
	$(CXX) -o $@ $^ ${CXXFLAGS}

refreshIter : refreshIter.cc
	$(CXX) -o $@ $^ ${CXXFLAGS}

stack: stack.cc
	$(CXX) -o $@ $^ ${CXXFLAGS}

str_assignOps : str_assignOps.cc
	$(CXX) -o $@ $^ ${CXXFLAGS}

substr : substr.cc
	$(CXX) -o $@ $^ ${CXXFLAGS}

useConvs : useConvs.cc
	$(CXX) -o $@ $^ ${CXXFLAGS}
