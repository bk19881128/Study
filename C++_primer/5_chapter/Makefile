include ../../Makefile.inc

TARGET = add_item2 doWhile guess ifgrades \
                  othercnt rangefor vowels


#DIRS_TARGET = 1 3 5 6 9 12 13 14 16 17 21 22 27 28 29 30 33 34

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

LOCFLAGS = -I../1_chapter
    
% : %.cc
	$(CXX) -o $@ $^ ${CXXFLAGS} ${LOCFLAGS}
