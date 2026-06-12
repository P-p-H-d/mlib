#!/bin/sh
name=/tmp/prepro-$USER/$$.c
param1=""
param2=""
if test "$SUB_CC" = "" ; then
    SUB_CC=cc
fi
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
echo "Preprocessing to $name using CC=$SUB_CC"
$SUB_CC $param1 -E |grep -v '^# ' > ${name}      \
    && clang-format -i ${name}              \
    && $SUB_CC $param2 ${name}
