include ../../Makefile.inc

TARGET = absInt add calc directcall \
                  fcnobj newcount-size readStr

#DIRS_TARGET =  5 13 18 46 56

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

LOCFLAGS = -I../6_chapter -I../1_chapter    

add: add.cc Sales_data.cc
	$(CXX) -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)

directcall: directcall.cc Sales_data.cc
	$(CXX) -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)

% : %.cc
	$(CXX) -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)
