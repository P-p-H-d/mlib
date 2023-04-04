#!/bin/sh
mkdir -p /tmp/prepro-$USER
name=/tmp/prepro-$USER/$$.c
param1=""
param2=""
while test $# -gt 0 ; do
    case $1 in
        -c) param2="$param2 -c" ;;
        *\.c)  param1="$param1 $1" ;;
        *) param1="$param1 $1" ; param2="$param2 $1" ;;
     esac
    shift
done
cc $param1 -E |grep -v '^#' > ${name}      \
    && perl -pi -e  's/\;/\;\n/g' ${name}    \
    && cc $param2 ${name}
