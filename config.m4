dnl $Id$
dnl config.m4 for extension pquery

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(pquery, for pquery support,
dnl Make sure that the comment is aligned:
dnl [  --with-pquery             Include pquery support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(pquery, whether to enable pquery support,
Make sure that the comment is aligned:
[  --enable-pquery           Enable pquery support])

if test "$PHP_PQUERY" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-pquery -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/pquery.h"  # you most likely want to change this
  dnl if test -r $PHP_PQUERY/$SEARCH_FOR; then # path given as parameter
  dnl   PQUERY_DIR=$PHP_PQUERY
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for pquery files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       PQUERY_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$PQUERY_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the pquery distribution])
  dnl fi

  dnl # --with-pquery -> add include path
  dnl PHP_ADD_INCLUDE($PQUERY_DIR/include)

  dnl # --with-pquery -> check for lib and symbol presence
  dnl LIBNAME=pquery # you may want to change this
  dnl LIBSYMBOL=pquery # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $PQUERY_DIR/$PHP_LIBDIR, PQUERY_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_PQUERYLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong pquery lib version or lib not found])
  dnl ],[
  dnl   -L$PQUERY_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(PQUERY_SHARED_LIBADD)

  PHP_ADD_EXTENSION_DEP(pquery, pcre)
  # PHP_ADD_EXTENSION_DEP(pquery, dom)
  PHP_NEW_EXTENSION(pquery, pquery.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  
  if test -z "$PHP_DEBUG"; then
    AC_ARG_ENABLE(debug,
      [--enable-debug compile with debugging system],
      [PHP_DEBUG=$enableval], 
      [PHP_DEBUG=no]
    )
  fi
fi
