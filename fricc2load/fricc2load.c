/*
   +----------------------------------------------------------------------+
   | FRICC2                                                               |
   +----------------------------------------------------------------------+
   | Author: sun bing <hoowa.sun@gmail.com>                               |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_fricc2load.h"
#include "fricc2_lib.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

/* For compatibility with PHP 8.X versions */
#ifndef TSRMLS_D
#define TSRMLS_D void
#define TSRMLS_DC
#define TSRMLS_C
#define TSRMLS_CC
#define TSRMLS_FETCH()
#endif

/*
	=======================================================================
	FRICC2LOAD Functions
	=======================================================================
*/
int fricc2load_fd_checkheader(FILE *fp)
{
   	char ftag[FRICCTAG_LEN + 1];

	memset(ftag, 0, sizeof(ftag));
	fread(ftag, sizeof(char), FRICCTAG_LEN, fp);

	return memcmp(ftag, FRICCTAG_STR, FRICCTAG_LEN);
}

char *fricc2load_fd_decrypt(FILE *fp, size_t *real_data_len)
{
	// file data
	struct stat fp_stat;
	size_t file_buf_len = 0;
	char *file_buf = NULL;
	// real
	char *real_data_buf = NULL;

	// from handle to file_data
	fstat(fileno(fp), &fp_stat);
	file_buf_len = fp_stat.st_size - FRICCTAG_LEN;
	file_buf = safe_emalloc(file_buf_len, sizeof(char), 0);
	fread(file_buf, sizeof(char), file_buf_len, fp);

	// decrypt and uncompress
	fricc2_lib_decrypt(file_buf,&file_buf_len);
	real_data_buf = fricc2_lib_zcodecom(ZDECOMPRESS, file_buf, file_buf_len, real_data_len); 

	// clean done
	efree(file_buf);

	return(real_data_buf);
}

ZEND_API zend_op_array *(*org_compile_file)(zend_file_handle *file_handle, int type TSRMLS_DC);
ZEND_API zend_op_array *fricc2load_compile_file(zend_file_handle *file_handle, int type TSRMLS_DC)
{
	// file handle
	FILE	*fp;
   	char	fname[32];
   	// real data
   	char	*real_data_buf = NULL;
   	size_t	real_data_len = 0;
   	// stream fixup used
   	char *tmp_buf = NULL;
   	size_t tmp_size = 0;

	// checking file name
   	memset(fname, 0, sizeof fname);
   	if (zend_is_executing(TSRMLS_C) && get_active_function_name(TSRMLS_C))
  		strncpy(fname, get_active_function_name(TSRMLS_C), sizeof fname - 2);
   	if (fname[0]) {
   		if ( strcasecmp(fname, "show_source") == 0
   			|| strcasecmp(fname, "highlight_file") == 0) {
   			return NULL;
   		}
   	}

   	// open file and decrypt and uncompress
	// When file is opened directly (type is ZEND_HANDLE_FP), check here.
	fp = fopen(file_handle->filename, "rb");
	if (!fp)
		return org_compile_file(file_handle, type);

   	// check header tag to ignore decrypt and uncompress
   	if (fricc2load_fd_checkheader(fp) != 0) {
   		fclose(fp);
   		return org_compile_file(file_handle, type);
   	}

	// decode real data
	real_data_buf = fricc2load_fd_decrypt(fp, &real_data_len);
	fclose(fp);
	if (real_data_buf == NULL) {
		//real_data_len = 0;
		return org_compile_file(file_handle, type TSRMLS_CC);
	} else if (real_data_len == 0) {
		if (real_data_buf != NULL) {
			efree(real_data_buf);
			real_data_buf = NULL;
		}
		return org_compile_file(file_handle, type TSRMLS_CC);
	}

	// replace with new buf
  	if ( zend_stream_fixup(file_handle, &tmp_buf, &tmp_size TSRMLS_CC) == FAILURE )
  		return NULL;
#if PHP_VERSION_ID < 70400
  	// verison < 7.4.0 can not use efree, but its come to memory leak.
  	//if (file_handle->handle.stream.mmap.buf != NULL)
	//	efree(file_handle->handle.stream.mmap.buf);
  	file_handle->handle.stream.mmap.buf = real_data_buf;
  	file_handle->handle.stream.mmap.len = real_data_len;
#else
  	if (file_handle->buf != NULL)
		efree(file_handle->buf);
 	file_handle->buf = real_data_buf;
 	file_handle->len = real_data_len;
#endif
  	file_handle->handle.stream.closer = NULL;

  	return org_compile_file(file_handle, type TSRMLS_CC);
}

/*
	=======================================================================
	PHP Extension API Define
	=======================================================================
*/
PHP_MINIT_FUNCTION(fricc2load)
{
//	CG(compiler_options) |= ZEND_COMPILE_EXTENDED_INFO;
	org_compile_file = zend_compile_file;
	zend_compile_file = fricc2load_compile_file;
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(fricc2load)
{
//	CG(compiler_options) |= ZEND_COMPILE_EXTENDED_INFO;
	zend_compile_file = org_compile_file;
	return SUCCESS;
}

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(fricc2load)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "fricc2load support", "enabled");
 	php_info_print_table_header(2, "fricc2load version", "2");
 	php_info_print_table_row(2, "URL", "http://github.com/hoowa/FRICC2");	
	php_info_print_table_end();
}
/* }}} */

/* {{{ fricc2load_module_entry
 */
zend_module_entry fricc2load_module_entry = {
	STANDARD_MODULE_HEADER,
	"fricc2load",					/* Extension name */
	NULL,							/* zend_function_entry */
	PHP_MINIT(fricc2load),			/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(fricc2load),		/* PHP_MSHUTDOWN - Module shutdown */
	NULL,							/* PHP_RINIT - Request initialization */
	NULL,							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(fricc2load),			/* PHP_MINFO - Module info */
	PHP_FRICC2LOAD_VERSION,			/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_FRICC2LOAD
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(fricc2load)
#endif
