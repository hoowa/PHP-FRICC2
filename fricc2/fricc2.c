/*
   +----------------------------------------------------------------------+
   | FRICC2                                                               |
   +----------------------------------------------------------------------+
   | Author: sun bing <hoowa.sun@gmail.com>                               |
   +----------------------------------------------------------------------+
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// #include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
// #include <unistd.h>
#include "fricc2_lib.h"

int main(int argc, char**argv)
{
	int option = -1;
	FILE *fp = NULL;
	struct stat fp_stat;
	size_t src_data_len = 0, dst_data_len = 0;
	char *src_data_buf = NULL, *dst_data_buf = NULL;

	// checking args input
	if (argc != 4) {
		fprintf(stderr, "Usage: %s -o [target] [source]\n",argv[0]);
		goto CLEAN_DONE;
	}
	option = strcmp(argv[1],"-o");
	if (option != 0) {
		fprintf(stderr, "Usage: %s -o [target] [source]\n",argv[0]);
		goto CLEAN_DONE;
	}

 	// load source
 	fp = fopen(argv[3], "r");
 	if (fp == NULL) {
 		fprintf(stderr, "File not found: %s\n", argv[3]);
 		goto CLEAN_DONE;
 	}
  	fstat(fileno(fp), &fp_stat);
  	src_data_len = fp_stat.st_size;
  	src_data_buf = (char*)malloc(src_data_len + FRICCTAG_LEN);
  	if(fread(src_data_buf, src_data_len, 1, fp)){
  	    fclose(fp);
  	}
  	fp = NULL;

  	//check source file
	 if (memcmp(src_data_buf, FRICCTAG_STR, FRICCTAG_LEN) == 0) {
  		fprintf(stderr, "Error Source was Encrypted: %s\n", argv[3]);
  		goto CLEAN_DONE;
  	}

	// target compress and encode
	dst_data_buf = fricc2_lib_zcodecom(ZENCOMPRESS, src_data_buf, src_data_len, &dst_data_len); 
	fricc2_lib_decrypt(dst_data_buf,&dst_data_len);

 	// save target
	fp = fopen(argv[2], "w");
 	if (fp == NULL) {
		fprintf(stderr, "Error Target File Create: %s\n", argv[2]);
		goto CLEAN_DONE;
	}
	fwrite(FRICCTAG_STR, FRICCTAG_LEN, 1, fp);
	fwrite(dst_data_buf, dst_data_len, 1, fp);
	fclose(fp);
	fp = NULL;
	fprintf(stderr, "Encrypted: %s\n", argv[2]);
	goto CLEAN_DONE;

CLEAN_DONE:
	if (fp != NULL) {
		//printf("fclose fp\n");
		fclose(fp);
	}
	if (src_data_buf != NULL) {
		//printf("free src_data_p\n");
		free(src_data_buf);
	}
	if (dst_data_buf != NULL) {
		//printf("free dst_data_p\n");
		free(dst_data_buf);
	}
	return 0;
}
