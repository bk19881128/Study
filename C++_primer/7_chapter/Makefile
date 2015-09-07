include ../../Makefile.inc

TARGET = add_item avg_price useAccount useScreen

DIRS_TARGET = 1 4 41 43

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
    
add_item : add_item.cc Sales_data.cc
	$(CXX) -o $@ $^ ${CXXFLAGS}

avg_price : avg_price.cc Sales_data.cc
	$(CXX) -o $@ $^ ${CXXFLAGS}

useAccount : Account.cc useAccount.cc
	$(CXX) -o $@ $^ ${CXXFLAGS}

useScreen : useScreen.cc
	$(CXX) -o $@ $< ${CXXFLAGS}

