/*
 * Copyright By Sun Bing <hoowa.sun@gmail.com>
 * Friccload2
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// #include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
// #include <unistd.h>
#include "../friccload2/friccload2.h"
#include "../friccload2/zencode.h"

int main(int argc, char**argv)
{
	int i = 0, option = -1, src_data_len = 0, dst_data_len = 0;
 	FILE *fp = NULL;
	struct stat src_stat;
 	char *src_data_p = NULL, *dst_data_p = NULL;
 	int cryptokey_len = sizeof friccload_cryptokey / 2;

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
 	fstat(fileno(fp), &src_stat);
 	src_data_len = src_stat.st_size;
 	src_data_p = (char*)malloc(src_data_len + FRICCTAG_LEN);
 	fread(src_data_p, src_data_len, 1, fp);
 	fclose(fp);	fp = NULL;

 	//check source file
	if (memcmp(src_data_p, FRICCTAG_STR, FRICCTAG_LEN) == 0) {
 		fprintf(stderr, "Error Source was Encrypted: %s\n", argv[3]);
 		goto CLEAN_DONE;
 	}

 	// replace encode
 	short tmp;
 	tmp = friccload_cryptokey[3];
 	friccload_cryptokey[3] = friccload_cryptokey[14];
 	friccload_cryptokey[14] = tmp;
 	tmp = friccload_cryptokey[7];
 	friccload_cryptokey[7] = friccload_cryptokey[9];
 	friccload_cryptokey[9] = tmp;

	// target compress and encode
	dst_data_p = zencode(src_data_p, src_data_len, &dst_data_len);
 	for (i=0; i<dst_data_len; i++) {
 		dst_data_p[i] = (char)friccload_cryptokey[(dst_data_len - i) % cryptokey_len] ^ (~(dst_data_p[i]));
 	}

 	// save target
	fp = fopen(argv[2], "w");
 	if (fp == NULL) {
 		fprintf(stderr, "Error Target File Create: %s\n", argv[2]);
 		goto CLEAN_DONE;
 	}
	fwrite(FRICCTAG_STR, FRICCTAG_LEN, 1, fp);
	fwrite(dst_data_p, dst_data_len, 1, fp);
 	fclose(fp); fp = NULL;
	fprintf(stderr, "Encrypted: %s\n", argv[2]);
	goto CLEAN_DONE;

CLEAN_DONE:
	if (fp != NULL) {
		//printf("fclose fp\n");
		fclose(fp);
	}
	if (src_data_p != NULL) {
		//printf("free src_data_p\n");
		free(src_data_p);
	}
	if (dst_data_p != NULL) {
		//printf("free dst_data_p\n");
		free(dst_data_p);
	}
	return 0;
}
