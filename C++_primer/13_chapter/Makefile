include ../../Makefile.inc

TARGET = FolderMain HasPtrUseCount \
                  HasPtrValueLike \
                  moveHP serial StrFolderMain \
                  useStrVec substr copyNoMove

DIRS_TARGET =  5 13 18

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

LOCFLAGS = -I../7_chapter -I../1_chapter    

useStrVec: StrVec.cc useStrVec.cc
	$(CXX) -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)

FolderMain: Folder.cc FolderMain.cc
	$(CXX) -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)

StrFolderMain: StrFolder.cc StrFolderMain.cc String.cc
	$(CXX) -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)

% : %.cc
	$(CXX) -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)
