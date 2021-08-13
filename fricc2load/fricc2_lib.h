/*
   +----------------------------------------------------------------------+
   | FRICC2                                                               |
   +----------------------------------------------------------------------+
   | Author: sun bing <hoowa.sun@gmail.com>                               |
   +----------------------------------------------------------------------+
*/
#ifndef FRICC2_LIB_H
#define FRICC2_LIB_H

#define FRICCTAG_STR	"hellokitty"
#define FRICCTAG_LEN	10
#define FRICCKEY 18447,763,892,7347,26797,24482,978,62,464,18,600,061,138,1321,1,1342

#define ZENCOMPRESS	0
#define ZDECOMPRESS	1
#define OUTBUFSIZ  100000

void fricc2_lib_decrypt(char *file_buf, size_t *file_buf_len);
char *fricc2_lib_zcodecom(int mode, char *inbuf, size_t inbuf_len, size_t *resultbuf_len);

#endif