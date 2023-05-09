#!/bin/bash

# LMFS WorkStation 2022 - Compilation file
#
# LICENSE SEE $ROOT/LICENSE

echo "Loading configuration..."

function getconfig(){
    python3 installconfig.py "$@"
}

PREFIX=$(getconfig prefix)

CP=$(getconfig cp)
CHMOD=$(getconfig chmod)
ID=$(getconfig id)

AR=$(getconfig ar)

CC=$(getconfig cc)
CFLAGS=$(getconfig cflags)
EXTRA_CFLAGS=$(getconfig extra_cflags)
OPT=$(getconfig optimize)
LIBS=$(getconfig dependences)
LDFLAGS=$(getconfig ldflags)
SOFLAGS=$(getconfig soflags)
MODULE_EXTRA_FLAGS=$(getconfig module_extra_flags)

LINK_FILE=$(getconfig shared_lib)

RUNTIME=$(getconfig requires)

OBJECT=$(getconfig compile_output)

WINDOWS=$(getconfig windows_flags)
DLNAME=$(getconfig dynamic_libraries_suffix)

LIBNAME=$(getconfig library_name_prefix)

echo -e "\e[91;1mYou should run this script under the project root\e[0m"

function _help(){
    echo "LMFS WorkStation Complation Script - 2022-2023
    Usage: build.sh [options...]
    Options:
       -default        Enable all
       -main           Enable main part
       -module         Enable modules part
       -install        Enable automatic install
       -debug          Append '-ggdb3' to \$CFLAGS
       -windows        Delete compiler-flag '-fPIC'
       -no-readline    Without GNU-Readline supports (default is enabled, dependence: libreadline)
       -no-optimize    Without compiler optimize
       -no-shared      Use libLMFSWS.a instead of libLMFSWS.so while linking
       -clang          Use Clang instead of GCC
       -version        Show version of the cloned LMFSWS source
       -no-pie         Build ELF EXEC Object for LMFSWS not ELF PIE-EXEC Object
       -strip          Automatic strip
       -enable-login   Enable secure section in LMFSWS (default is disabled, dependence: openssl)
       -sanitizer      Use address sanitizer
       -help           Show this message
    "
}

enable_login=$(getconfig enable_login)

function _start(){
    main=no
    module=no
    install=no

    if [ x"$@" = x"" ]; then
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
            OPT=""
        elif [ "$i" = "-version" ]; then
            cat VERSION
        elif [ "$i" = "-no-readline" ]; then
            LIBS=
        elif [ "$i" = "-no-optimize" ]; then
            OPT=""
        elif [ "$i" = "-no-shared" ]; then
            LINK_FILE="libLMFSWS.a"
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
            LIBNAME="bin"
            OBJECT="LMFSWS.exe"
        elif [ "$i" = "-enable-login" ]; then
            enable_login=yes
            CFLAGS="$CFLAGS -DENABLE_LOGIN"
        elif [ "$i" = "-sanitizer" ]; then
            CFLAGS="$CFLAGS -fsanitize=address,leak -fno-omit-frame-pointer"
        elif [ "$i" = "-help" ]; then
            _help
            exit 0
        else
            echo -e "\033[91;1mError\033[0m Unrecognized option \`$i'"
            _help
            exit 127
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

flags="$@"

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
    ${CC} $CFLAGS $EXTRA_CFLAGS -I.. ../src/"'$i'" -o "'$i'".o -c -DPREFIX=\"$PREFIX\"
done"

    for i in $ALLCFILES
    do
        run ${CC} $CFLAGS $EXTRA_CFLAGS -I.. ../src/$i -o $i.o -c -DPREFIX=\"$PREFIX\"
    done

    run ${CC} $EXTRA_CFLAGS $LDFLAGS *.o $SOFLAGS -o libLMFSWS"$DLNAME" -Wno-unused-command-line-argument
    run ${AR} rcs libLMFSWS.a *.o

    run ${CC} $CFLAGS -I.. $LDFLAGS ../src/LMFSWorkStation.c $LINK_FILE -o $OBJECT -DCOMPILATION=\""$flags"\" -DPREFIX=\"$PREFIX\"
}

function part_module(){
    if [ -d "build" ]; then
        run cd build
    fi

    # MODULES
    runNoEXIT mkdir modules
    run ${CP} ../module/*.mode modules

    ## Input - Required
    run ${CC} ../module/Input.c ${SOFLAGS} -o modules/Input.so ${LIBS} $CFLAGS $MODULE_EXTRA_FLAGS $EXTRA_CFLAGS ${RUNTIME} -DPREFIX=\"$PREFIX\"

    ## Command Line - Required
    run ${CC} ../module/cmdline.c ${SOFLAGS} -o modules/cmdline.so $CFLAGS $MODULE_EXTRA_FLAGS $EXTRA_CFLAGS ${RUNTIME} -DPREFIX=\"$PREFIX\"

    ## Extenal Modules User Mode Loader - Required
    run ${CC} ../module/register.c $LINK_FILE ${SOFLAGS} -o modules/register.so $CFLAGS $MODULE_EXTRA_FLAGS $EXTRA_CFLAGS ${RUNTIME} -DPREFIX=\"$PREFIX\"

    ## Core commands - Required
    run ${CC} ../module/tools.c $LINK_FILE ${SOFLAGS} -I.. -o modules/tools.so $CFLAGS $MODULE_EXTRA_FLAGS $EXTRA_CFLAGS ${RUNTIME} -DPREFIX=\"$PREFIX\"

    if [ x$enable_login = x"yes" ]; then
        # Security section
        run ${CP} ../extra/login.mode modules
        run ${CC} ../extra/login.c $LINK_FILE ${SOFLAGS} -o modules/login.so $CFLAGS $MODULE_EXTRA_FLAGS $EXTRA_CFLAGS ${RUNTIME} -DPREFIX=\"$PREFIX\" -lcrypto -Wno-deprecated-declarations -Wno-pointer-sign
    fi

}

function part_install(){
    # INSTALLATION
    if [ -d "build" ]; then
        run cd build
    fi

    runNoEXIT mkdir -p "$PREFIX"/lib/LMFSWSModules
    runNoEXIT mkdir -p "$PREFIX"/bin
    runNoEXIT mkdir -p "$PREFIX"/share/licenses/LMFSWS
    if [ x$enable_login = x"yes" ]; then
        runNoEXIT mkdir -p "$PREFIX"/etc/lmfsws.d
        runNoEXIT touch "$PREFIX"/etc/lmfsws.d/shadow
        runNoEXIT chmod 0644 "$PREFIX"/etc/lmfsws.d/shadow
        echo "echo "$($ID -u)":x > $PREFIX/etc/lmfsws.d/shadow"
        echo $($ID -u):x > $PREFIX/etc/lmfsws.d/shadow
    fi
    run ${CP} -rf modules/* "$PREFIX"/lib/LMFSWSModules
    run ${CP} -rf *.a "$PREFIX"/lib
    run ${CP} -rf *"$DLNAME" "$PREFIX"/$LIBNAME
    run ${CP} -rf ${OBJECT} "$PREFIX"/bin
    run ${CHMOD} 755 "$PREFIX"/bin/${OBJECT}
    run ${CP} -rf LICENSE "$PREFIX"/share/licenses/LMFSWS

    runNoEXIT mkdir -p "$PREFIX"/include/LMFSWS
    run ${CP} -rf ../include/* "$PREFIX"/include/LMFSWS
}

_start "$@"

