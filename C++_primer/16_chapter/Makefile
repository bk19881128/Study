include ../../Makefile.inc

TARGET = Array array-parms4 \
                  compare compareDef compareSpec \
                  debug multiset overloaded overSpec qm \
                  SP special specialized \
                  useBlob useChcking Vecmain

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

LOCFLAGS = -I../6_chapter -I../12_chapter    

compareDef: compareDef.cc ../14_chapter/Sales_data.cc
	$(CXX) -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)

qm: qm.cc build.cc
	$(CXX) -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)

specialized: debug.cc
	$(CXX) -D SPECIALIZED -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)

overloaded: debug.cc
	$(CXX) -D OVERCHAR -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)

debug: debug.cc
	$(CXX) -D DEBUG -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)

multiset: multiset.cc Sales_data.cc
	$(CXX) -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)

sample: sample.cc Quote.cc
	$(CXX) -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)

% : %.cc
	$(CXX) -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)
