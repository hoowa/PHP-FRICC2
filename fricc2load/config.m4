PHP_ARG_ENABLE([fricc2load],
  [whether to enable fricc2load support],
  [AS_HELP_STRING([--enable-fricc2load],
    [Enable fricc2load support])],
  [no])

if test -z "$PHP_ZLIB_DIR"; then
  PHP_ARG_WITH([zlib-dir],
    [for the location of libz],
    [AS_HELP_STRING([[--with-zlib-dir[=DIR]]],
      [fricc2load: Set the path to libz install prefix])],
    [no],
    [no])
fi

if test "$PHP_FRICC2LOAD" != "no"; then

  dnl PKG_CHECK_MODULES([ZLIB], [zlib >= 1.0.0.0])
  dnl AC_DEFINE(HAVE_FRICC2LOAD, 1, [ Have fricc2load support ])

  PHP_CHECK_LIBRARY(z, gzgets, [
    AC_DEFINE(HAVE_FRICC2LOAD,1,[ Have fricc2load support ])
  ],[
    AC_MSG_ERROR(HAVE_FRICC2LOAD extension requires gzgets in zlib)
  ])

  PHP_EVAL_INCLINE($ZLIB_CFLAGS)
  PHP_EVAL_LIBLINE($ZLIB_LIBS, FRICC2LOAD_SHARED_LIBADD)

  PHP_NEW_EXTENSION(fricc2load, fricc2load.c fricc2_lib.c, $ext_shared,, -DFRICC2_INPHP_COMPILE=1)
fi
