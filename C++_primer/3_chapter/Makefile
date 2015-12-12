include ../../Makefile.inc

TARGET = add_using arrayScores cctype \
                  char_repl cstring_hdr getline \
                  hexify multidim \
                  ptr_traversal2 square strcompare \
                  string_add string_io string_io2 \
                  string_size string_size2 string_size3 \
                  vec_decls vecStrings2 vecScores \
                  vecSubs word_echo


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
