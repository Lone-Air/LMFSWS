/*
 * LMFS WorkStation - Configuration
 *
 * LICENSE SEE $ROOT/LICENSE
 */

#ifndef LMFSWS_CFG_H
#define LMFSWS_CFG_H

#define LIBNAME "/lib/"
#define BINNAME "/bin/"

#define ETCNAME PREFIX"/etc/lmfsws.d/"
#ifdef ENABLE_LOGIN
  #define SHADOWPATH ETCNAME"/shadow"
#else
  #define SHADOWPATH ""
#endif

#define MODNAME LIBNAME"/LMFSWSModules/"

#define MODULE PREFIX MODNAME

#endif

