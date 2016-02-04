include ../../Makefile.inc

TARGET = badgetc bits  \
                  game getc getput manips \
                  normalInts no-skipws point \
                  prec rand1 rand2 rand4 \
                  rand6 seed seek setw \
                  skipws tuple

#DIRS_TARGET =  34 35 37 38 42 43 45 50

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

LOCFLAGS = -I../14_chapter    

#findbook: findbook.cc ../14/Sales_data.cc
#	$(CXX) -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)

% : %.cc
	$(CXX) -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)
