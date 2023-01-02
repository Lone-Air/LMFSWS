/*
 * LMFS WorkStation - Configuration
 *
 * LICENSE SEE $ROOT/LICENSE
 */

#ifndef LMFSWS_CFG_H
#define LMFSWS_CFG_H

#define PREFIX "/usr"

#define LIBNAME "/lib/"
#define BINNAME "/bin/"

#define MODNAME LIBNAME"/LMFSWSModules/"

#define MODULE PREFIX MODNAME

#ifndef __WIN32__
#define HAVE_READLINE
#endif

#endif

