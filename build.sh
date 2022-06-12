#!/bin/bash

# LMFS WorkStation 2022 - Compilation file
#
# LICENSE SEE $ROOT/LICENSE

PREFIX=/usr

CP=cp
CHMOD=chmod

CC=gcc
CFLAGS=-std=c11
LIBS=-lreadline
LDFLAGS=-pie
SOFLAGS="-fPIC -shared"

OBJECT=LMFSWS

echo -e "\e[91;1mYou should run this script under the project root\e[0m"

function run() {
    echo -e "$@"
    "$@"
}

run mkdir build
run cd src
FILES=$(ls *.c)
run cd ..
run cd build

# MAIN
#run ${CC} ../src/*.c -I../include -o LMFSWS $CFLAGS
for i in $FILES
do
    run ${CC} $CFLAGS ../src/$i -o $i.o -c -fPIC
done

run ${CC} $LDFLAGS *.o -o ${OBJECT}

# MODULES
run mkdir modules
run ${CP} ../module/* modules

## Input - Required
run ${CC} ../module/Input.c -I../include ${SOFLAGS} -o modules/Input.so ${LIBS} $CFLAGS

## Command Line - Required
run ${CC} ../module/cmdline.c -I../include ${SOFLAGS} -o modules/cmdline.so $CFLAGS

## Version information - Required
run ${CC} ../module/version.c -I.. -I../include ${SOFLAGS} -o modules/version.so $CFLAGS

## Extenal Modules User Mode Loader - Required
run ${CC} ../module/register.c command-register.c.o module-loader.c.o -I../include ${SOFLAGS} -o modules/register.so $CFLAGS

## Text Printer - External
run ${CC} ../module/echo.c -I../include ${SOFLAGS} -o modules/echo.so $CFLAGS

# INSTALLATION
run mkdir -p $PREFIX/lib/LMFSWSModules
run mkdir -p $PREFIX/bin
run ${CP} modules/* $PREFIX/lib/LMFSWSModules
run ${CP} ${OBJECT} $PREFIX/bin
run ${CHMOD} 755 $PREFIX/bin/${OBJECT}

run mkdir $PREFIX/include/LMFSWS
run ${CP} -rf ../include/* $PREFIX/include/LMFSWS

