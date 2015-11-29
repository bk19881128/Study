include ../../Makefile.inc

TARGET = add_itemV2 add_item buf clearIO fileIO sstream

DIRS_TARGET = 1 4 5 6 10

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
    

add_itemV2 : add_itemV2.cc ../7_chapter/Sales_data.cc
	$(CXX) -o $@ $^ ${CXXFLAGS}


add_item : add_item.cc ../7_chapter/Sales_data.cc
	$(CXX) -o $@ $^ ${CXXFLAGS}

buf : buf.cc
	$(CXX) -o $@ $^ ${CXXFLAGS}

clearIO : clearIO.cc
	$(CXX) -o $@ $^ ${CXXFLAGS}

fileIO : fileIO.cc
	$(CXX) -o $@ $^ ${CXXFLAGS}

sstream : sstream.cc
	$(CXX) -o $@ $^ ${CXXFLAGS}

