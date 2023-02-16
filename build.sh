#!/bin/bash

# LMFS WorkStation 2022 - Compilation file
#
# LICENSE SEE $ROOT/LICENSE

PREFIX=/usr

CP=cp
CHMOD=chmod

AR=ar

CC=gcc
CFLAGS="-std=c11 -I../include -Wall -Wno-deprecated-non-prototype -DPREFIX=\"$PREFIX\""
EXTRA_CFLAGS="-fPIC"
OPT="-O3"
LIBS="-DHAVE_READLINE -lreadline"
LDFLAGS="-pie -fPIE"
SOFLAGS="-shared"
MODULE_EXTRA_FLAGS=""

LINK_FILE="libLMFSWS.so"

OBJECT=LMFSWS

WINDOWS=
DLNAME=".so"

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
       -link-static    Use libLMFSWS.a to link LMFSWS not libLMFSWS.so
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
            EXTRA_CFLAGS=""
            WINDOWS=yes
            LINK_FILE="libLMFSWS.dll"
            DLNAME=".dll"
        elif [ "$i" = "-link-static" ]; then
            LINK_FILE="libLMFSWS.a"
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
    run ${CP} LICENSE build
    run cd src
    ALLFILES="command-register.o module-loader.o cli.o"
    ALLCFILES="command-register.c module-loader.c cli.c"
    run cd ..
    run cd build

    # MAIN
    tell "for i in $ALLCFILES
do
    ${CC} $CFLAGS $EXTRA_CFLAGS -I.. ../src/"'$i'" -o "'$i'".o -c
done"

    for i in $ALLCFILES
    do
        run ${CC} $CFLAGS $EXTRA_CFLAGS -I.. ../src/$i -o $i.o -c
    done

    run ${CC} $EXTRA_CFLAGS $LDFLAGS *.o $SOFLAGS -o libLMFSWS"$DLNAME"
    run ${AR} rcs libLMFSWS.a *.o

    run ${CC} $CFLAGS -I.. $LDFLAGS ../src/LMFSWorkStation.c $LINK_FILE -o $OBJECT
}

function part_module(){
    if [ -d "build" ]; then
        run cd build
    fi

    # MODULES
    runNoEXIT mkdir modules
    run ${CP} ../module/*.mode modules

    ## Input - Required
    run ${CC} ../module/Input.c ${SOFLAGS} -o modules/Input.so ${LIBS} $CFLAGS $MODULE_EXTRA_FLAGS $EXTRA_CFLAGS

    ## Command Line - Required
    run ${CC} ../module/cmdline.c ${SOFLAGS} -o modules/cmdline.so $CFLAGS $MODULE_EXTRA_FLAGS $EXTRA_CFLAGS

    ## Extenal Modules User Mode Loader - Required
    run ${CC} ../module/register.c $LINK_FILE ${SOFLAGS} -o modules/register.so $CFLAGS $MODULE_EXTRA_FLAGS $EXTRA_CFLAGS

    ## Core commands - Required
    run ${CC} ../module/tools.c $LINK_FILE ${SOFLAGS} -I.. -o modules/tools.so $CFLAGS $MODULE_EXTRA_FLAGS $EXTRA_CFLAGS

}

function part_install(){
    # INSTALLATION
    if [ -d "build" ]; then
        run cd build
    fi

    runNoEXIT mkdir -p $PREFIX/lib/LMFSWSModules
    runNoEXIT mkdir -p $PREFIX/bin
    runNoEXIT mkdir -p $PREFIX/share/licenses/LMFSWS
    run ${CP} -rf modules/* $PREFIX/lib/LMFSWSModules
    run ${CP} -rf *.a *.so $PREFIX/lib
    run ${CP} -rf ${OBJECT} $PREFIX/bin
    run ${CHMOD} 755 $PREFIX/bin/${OBJECT}
    run ${CP} -rf LICENSE $PREFIX/share/licenses/LMFSWS

    runNoEXIT mkdir -p $PREFIX/include/LMFSWS
    run ${CP} -rf ../include/* $PREFIX/include/LMFSWS
}

_start "$@"

