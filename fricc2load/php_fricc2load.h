/*
   +----------------------------------------------------------------------+
   | FRICC2                                                               |
   +----------------------------------------------------------------------+
   | Author: sun bing <hoowa.sun@gmail.com>                               |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_FRICC2LOAD_H
# define PHP_FRICC2LOAD_H

extern zend_module_entry fricc2load_module_entry;
# define phpext_fricc2load_ptr &fricc2load_module_entry

# define PHP_FRICC2LOAD_VERSION "2.0.0"

# if defined(ZTS) && defined(COMPILE_DL_FRICC2LOAD)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_FRICC2LOAD_H */
