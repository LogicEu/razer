#!/bin/bash

name=razer
cc=gcc
src=*.c
std='-std=c99'

if echo "$OSTYPE" | grep -q "linux"; then
    rflag="-Wl,--whole-archive"
    lflag="-Wl,--no-whole-archive"
fi

flags=(
    -Wall
    -Wextra
    -O2
)

inc=(
    -Iinclude/
    -Iimgtool/
    -Ifract/
    -Iphoton/
    -Imass/
    -Iutopia/
    -I.
)

lib=(
    -Llib/
    $rflag
    -limgtool
    -lfract
    -lphoton
    -lmass
    -lutopia
    $lflag
    -lz
    -lpng
    -ljpeg
)

mac=(
    #-mmacosx-version-min=10.9
)

linux=(
    -lm
    #-lpthread
    #-D_POSIX_C_SOURCE=199309L
)

fail() {
    echo "Use with -comp to compile or -run to compile and execute"
    exit
}

lib_build() {
    pushd $1/ && ./build.sh $2 && mv *.a ../lib/ && popd
}

build() {
    mkdir lib/
    lib_build imgtool -slib
    lib_build photon -s
    lib_build fract -s
    lib_build mass -s
    lib_build utopia -slib
}

comp() {
    if echo "$OSTYPE" | grep -q "darwin"; then
        $cc $src -o $name $std ${flags[*]} ${mac[*]} ${inc[*]} ${lib[*]}
    elif echo "$OSTYPE" | grep -q "linux"; then
        $cc $src -o $name $std ${flags[*]} ${inc[*]} ${lib[*]} ${linux[*]}
    else
        echo "OS not supported yet" && exit
    fi
}

clean() {
    rm -r lib/ && rm $name
}

case "$1" in
    "-build")
        build;;
    "-comp")
        comp;;
    "-run")
        comp && ./$name "$@";;
    "-clean")
        clean;;
    "-all")
        build && comp && ./$name "$@";;
    *)
        fail;;
esac
