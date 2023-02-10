#!/bin/bash

# LMFS WorkStation 2022 - Compilation file
#
# LICENSE SEE $ROOT/LICENSE

PREFIX=/usr

CP=cp
CHMOD=chmod

CC=gcc
CFLAGS='-std=c11 -I../include -Wall -Wno-deprecated-non-prototype -fPIC'
OPT="-O3"
LIBS="-DHAVE_READLINE -lreadline"
LDFLAGS="-pie -fPIE"
SOFLAGS="-shared"
MODULE_EXTRA_FLAGS=""

OBJECT=LMFSWS

echo -e "\e[91;1mYou should run this script under the project root\e[0m"

function _help(){
    echo "LMFS WorkStation Complation Script - 2022
    Usage: build.sh [options...]
    Options:
       -default        Enable all
       -main           Enable main part
       -module         Enable modules part
       -install        Enable automatic install
       -debug          Append '-ggdb3' to \$CFLAGS
       -windows        Delete compiler-flag '-fPIC'
       -no-readline    Without GNU-Readline supports
       -no-optimize    Without compiler optimize
       -clang          Use Clang instead of GCC
       -version        Show version of the cloned LMFSWS source
       -no-pie         Build ELF EXEC Object for LMFSWS not ELF PIE-EXEC Object
       -strip          Automatic strip
       -help           Show this message
    "
}

function _start(){
    main=no
    module=no
    install=no

    if [ -z "$@" ]; then
        main=yes
        module=yes
        install=yes
    fi

    for i in $@
    do
        if [ "$i" = "-default" ]; then
            main=yes
            module=yes
            install=yes
        elif [ "$i" = "-main" ]; then
            main=yes
        elif [ "$i" = "-module" ]; then
            module=yes
        elif [ "$i" = "-install" ]; then
            install=yes
        elif [ "$i" = "-debug" ]; then
            CFLAGS="$CFLAGS -ggdb3"
        elif [ "$i" = "-version" ]; then
            cat VERSION
        elif [ "$i" = "-no-readline" ]; then
            LIBS=
        elif [ "$i" = "-no-optimize" ]; then
            OPT=""
        elif [ "$i" = "-clang" ]; then
            CC="clang"
        elif [ "$i" = "-no-pie" ]; then
            LDFLAGS="-no-pie"
        elif [ "$i" = "-strip" ]; then
            LDFLAGS="$LDFLAGS -s"
            MODULE_EXTRA_FLAGS="-s"
        elif [ "$i" = "-windows" ]; then
            SOFLAGS=${SOFLAGS:5}
        elif [ "$i" = "-help" ]; then
            _help
        else
            echo -e "\033[91;1mError\033[0m Unrecognized option \`$i'"
            _help
        fi
    done

    CFLAGS="$CFLAGS $OPT"

    if [ "$main" = "yes" ]; then
        part_main
    fi

    if [ "$module" = "yes" ]; then
        part_module
    fi

    if [ "$install" = "yes" ]; then
        part_install
    fi


}

function run() {
    echo -e "$@"
    "$@" && return
    exit 1
}

function runNoEXIT(){
    echo -e "$@"
    "$@"
}

function tell(){
    echo -e "$@"
}

function part_main(){
    rm -rf build
    runNoEXIT mkdir build
    run cd src
    FILES=$(ls *.c)
    run cd ..
    run cd build

    # MAIN
    tell 'for i in $FILES
do
    run ${CC} $CFLAGS -I.. ../src/$i -o $i.o -c
done'

    for i in $FILES
    do
        run ${CC} $CFLAGS -I.. ../src/$i -o $i.o -c
    done

    run ${CC} $LDFLAGS *.o -o ${OBJECT}
}

function part_module(){
    if [ -d "build" ]; then
        run cd build
    fi

    # MODULES
    runNoEXIT mkdir modules
    run ${CP} ../module/*.mode modules

    ## Input - Required
    run ${CC} ../module/Input.c ${SOFLAGS} -o modules/Input.so ${LIBS} $CFLAGS $MODULE_EXTRA_FLAGS

    ## Command Line - Required
    run ${CC} ../module/cmdline.c ${SOFLAGS} -o modules/cmdline.so $CFLAGS $MODULE_EXTRA_FLAGS

    ## Extenal Modules User Mode Loader - Required
    run ${CC} ../module/register.c command-register.c.o module-loader.c.o ${SOFLAGS} -o modules/register.so $CFLAGS $MODULE_EXTRA_FLAGS

    ## Core commands - Required
    run ${CC} ../module/tools.c ${SOFLAGS} -I.. -o modules/tools.so $CFLAGS $MODULE_EXTRA_FLAGS

}

function part_install(){
    # INSTALLATION
    if [ -d "build" ]; then
        run cd build
    fi

    runNoEXIT mkdir -p $PREFIX/lib/LMFSWSModules
    runNoEXIT mkdir -p $PREFIX/bin
    run ${CP} modules/* $PREFIX/lib/LMFSWSModules
    run ${CP} ${OBJECT} $PREFIX/bin
    run ${CHMOD} 755 $PREFIX/bin/${OBJECT}

    runNoEXIT mkdir $PREFIX/include/LMFSWS
    run ${CP} -rf ../include/* $PREFIX/include/LMFSWS
}

_start "$@"

