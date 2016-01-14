include ../../../Makefile.inc

TARGET = sales_data_to_string_and_double

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

sales_data_to_string_and_double: sales_data_to_string_and_double.cc Sales_data.cc
	$(CXX) -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)



