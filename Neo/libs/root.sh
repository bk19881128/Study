#!/bin/bash
MYROOT=`/bin/pwd | /bin/sed \
          -e 's#/src.*##' \
          -e 's#/tools.*##' \
          -e 's#/build.*##' \
       `
echo $MYROOT
