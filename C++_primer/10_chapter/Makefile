include ../../Makefile.inc

TARGET = absInt accum accum4 \
                  avg_price elimDups equal \
                  equiv-istream-iter inserters \
                  iostream_iter newcount-size \
                  ostreamIter2 rcomma \
                  reverse reverse-iter rev-iters \
                  sortSI use_find use-istream-iter
#!!!!!!bind2 not OK!!!!!

DIRS_TARGET = 1 3 5 6 9 12 13

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
    

% : %.cc
	$(CXX) -o $@ $^ ${CXXFLAGS}
