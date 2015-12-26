# Makefile to build all programs in all subdirectories
#
# DIRS is a list of all subdirectories containing makefiles
#
#

DIRS = source_code 4_chapter 5_chapter 6_chapter 7_chapter 8_chapter 9_chapter 10_chapter 11_chapter 12_chapter


BUILD_DIRS = ${DIRS}

all:
	@ for dir in ${BUILD_DIRS}; do (cd $${dir}; ${MAKE}) ; done

allgen:
	@ for dir in ${BUILD_DIRS}; do (cd $${dir}; ${MAKE} allgen) ; done

clean:
	@ for dir in ${BUILD_DIRS}; do (cd $${dir}; ${MAKE} clean) ; done

