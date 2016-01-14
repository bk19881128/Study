include ../../Makefile.inc

TARGET = andQueryTest and_orQueryTest \
                  Basket_main wordQueryTest \
                  sample scope useQuote \
                  vecBasket virtualsNscope

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

wordQueryTest: Query.cc wordQueryTest.cc ../12_chapter/TextQuery.cc get_print.cc
	$(CXX) -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)

andQueryTest: Query.cc andQueryTest.cc ../12_chapter/TextQuery.cc get_print.cc
	$(CXX) -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)

and_orQueryTest: Query.cc and_orQueryTest.cc ../12_chapter/TextQuery.cc get_print.cc
	$(CXX) -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)

useQuote: useQuote.cc Quote.cc
	$(CXX) -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)

vecBasket: Basket.cc vecBasket.cc Quote.cc
	$(CXX) -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)

Basket_main: Basket.cc Basket_main.cc Quote.cc
	$(CXX) -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)

sample: sample.cc Quote.cc
	$(CXX) -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)

% : %.cc
	$(CXX) -o $@ $^ ${CXXFLAGS} $(LOCFLAGS)
