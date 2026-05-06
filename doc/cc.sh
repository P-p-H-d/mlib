#!/bin/sh
name=/tmp/prepro-$USER/$$.c
param1=""
param2=""
while test $# -gt 0 ; do
    case $1 in
        -c) param2="$param2 -c" ;;
        -o) param2="$param2 -o $2" ; shift ;;
        *\.c)  param1="$param1 $1" ; name=/tmp/prepro-$USER/$1 ;;
        *) param1="$param1 $1" ; param2="$param2 $1" ;;
     esac
    shift
done
d=$(dirname $name)
mkdir -p $d
echo "Preprocessing to $name"
cc $param1 -E |grep -v '^# ' > ${name}      \
    && clang-format -i ${name}              \
    && cc $param2 ${name}
