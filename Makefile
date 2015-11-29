# Makefile to build all programs in all subdirectories
#
# DIRS is a list of all subdirectories containing makefiles
#
#

DIRS = Algorithm \
       Introduction \
       C++_primer \
       Interview \
       Neo \
       TLPI \

BUILD_DIRS = ${DIRS}

all:
	 @ for dir in ${BUILD_DIRS}; do (cd $${dir}; ${MAKE}) ; done

allgen:
	 @ for dir in ${BUILD_DIRS}; do (cd $${dir}; ${MAKE} allgen) ; done

clean:
	 @ for dir in ${BUILD_DIRS}; do (cd $${dir}; ${MAKE} clean) ; done
