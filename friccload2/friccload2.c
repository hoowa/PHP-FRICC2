/*
 * Copyright By Sun Bing <hoowa.sun@gmail.com>
 * Friccload2
 */

#include "php.h"
#include "php_ini.h"
#include "ext/standard/file.h"
#include "ext/standard/info.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "friccload2.h"
#include "zencode.h"
#include "zencode.c" // why php7 can not auto include zencode.c ?

PHP_MINIT_FUNCTION(friccload2);
PHP_MSHUTDOWN_FUNCTION(friccload2);
PHP_MINFO_FUNCTION(friccload2);

// open users php code function
FILE *friccload2_ext_fopen(FILE *fp)
{
	struct stat fp_stat;
	int i, src_data_len, dst_data_len;
	char *src_data_p, *dst_data_p;
	int cryptokey_len = sizeof friccload_cryptokey / 2;

	// read source
	fstat(fileno(fp), &fp_stat);
	src_data_len = fp_stat.st_size - FRICCTAG_LEN;
	src_data_p = (char*)malloc(src_data_len);
	fread(src_data_p, src_data_len, 1, fp);
	fclose(fp);

 	// replace encode
 	short tmp;
 	tmp = friccload_cryptokey[3];
 	friccload_cryptokey[3] = friccload_cryptokey[14];
 	friccload_cryptokey[14] = tmp;
 	tmp = friccload_cryptokey[7];
 	friccload_cryptokey[7] = friccload_cryptokey[9];
 	friccload_cryptokey[9] = tmp;

	// source decode and uncompress to dst_data_p
	for (i=0; i<src_data_len; i++) {
		src_data_p[i] = (char)friccload_cryptokey[(src_data_len - i) % cryptokey_len] ^ (~(src_data_p[i]));
	}
	dst_data_p = zdecode(src_data_p, src_data_len, &dst_data_len);

	// create new fp save dst_data_p
	fp = tmpfile();
	fwrite(dst_data_p, dst_data_len, 1, fp);

	// free source and dst data
	free(src_data_p);
	free(dst_data_p);

	rewind(fp);
	return fp;
}

ZEND_API zend_op_array *(*org_compile_file)(zend_file_handle *file_handle, int type TSRMLS_DC);

ZEND_API zend_op_array *friccload2_compile_file(zend_file_handle *file_handle, int type TSRMLS_DC)
{
	FILE	*fp;
	char	fname[32];
	char	ftag[FRICCTAG_LEN + 1];

	if (!file_handle || !file_handle->filename || strstr(file_handle->filename, ".phar") || strstr(file_handle->filename, "phar://")) {
	 return org_compile_file(file_handle, type);
	}

	memset(fname, 0, sizeof fname);
	if (zend_is_executing(TSRMLS_C)) {
		if (get_active_function_name(TSRMLS_C)) {
			strncpy(fname, get_active_function_name(TSRMLS_C), sizeof fname - 2);
		}
	}
	if (fname[0]) {
		if ( strcasecmp(fname, "show_source") == 0
			|| strcasecmp(fname, "highlight_file") == 0) {
			return NULL;
		}
	}

	fp = fopen(file_handle->filename, "rb");
	if (!fp) {
		fp = zend_fopen(file_handle->filename, &file_handle->opened_path);
		if (!fp) {
			return org_compile_file(file_handle, type);
		}
	}

	// check header tag is exist
	memset(ftag, 0, sizeof ftag);
	fread(ftag, FRICCTAG_LEN, 1, fp);
	if (memcmp(ftag, FRICCTAG_STR, FRICCTAG_LEN) != 0) {
		fclose(fp);
		return org_compile_file(file_handle, type);
	}

	// hook to open file with friccload2_ext_fopen
	if (file_handle->type == ZEND_HANDLE_FP) fclose(file_handle->handle.fp);
	#ifdef ZEND_HANDLE_FD
		if (file_handle->type == ZEND_HANDLE_FD) close(file_handle->handle.fd);
	#endif
	file_handle->handle.fp = friccload2_ext_fopen(fp);
	file_handle->type = ZEND_HANDLE_FP;

	return org_compile_file(file_handle, type);
}

zend_module_entry friccload2_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"friccload2",
	NULL,
	PHP_MINIT(friccload2),
	PHP_MSHUTDOWN(friccload2),
	NULL,
	NULL,
	PHP_MINFO(friccload2),
#if ZEND_MODULE_API_NO >= 20010901
	"1.5.0", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(friccload2);

PHP_MINFO_FUNCTION(friccload2)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "friccload2 support", "enabled");
	php_info_print_table_header(2, "friccload2 version", "2.0");
	php_info_print_table_end();
}

PHP_MINIT_FUNCTION(friccload2)
{
	CG(compiler_options) |= ZEND_COMPILE_EXTENDED_INFO;
	org_compile_file = zend_compile_file;
	zend_compile_file = friccload2_compile_file;
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(friccload2)
{
	CG(compiler_options) |= ZEND_COMPILE_EXTENDED_INFO;
	zend_compile_file = org_compile_file;
	return SUCCESS;
}
