dnl config.m4 for extension friccload2
dnl don't forget to call PHP_EXTENSION(friccload2)
dnl If your extension references something external, use with:

PHP_ARG_WITH(friccload2, for friccload2 support,
dnl Make sure that the comment is aligned:
[  --with-friccload2	Include friccload2 support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(friccload2, whether to enable friccload2 support,
dnl Make sure that the comment is aligned:
[  --enable-friccload2	Enable friccload2 support])

if test "$PHP_friccload2" != "no"; then
  dnl Action..
  PHP_NEW_EXTENSION(friccload2, friccload2.c, $ext_shared)
fi
