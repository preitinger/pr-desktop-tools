#!/bin/sh

path=$1
name=$2

usage() {
    echo "usage: $0 <path> <name>"
}

if [ "$path" = "" -o "$name" = "" ]
then
    usage
    exit 1
fi


echo "#pragma once

#include \"utils/utils.H\"

class ${name} {
public:
};
" >>${path}/${name}.H

echo "#include \"${name}.H\"
#include \"utils/utils.H\"

" >>${path}/${name}.cxx