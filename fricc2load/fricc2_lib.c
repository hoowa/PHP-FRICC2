/*
   +----------------------------------------------------------------------+
   | FRICC2                                                               |
   +----------------------------------------------------------------------+
   | Author: sun bing <hoowa.sun@gmail.com>                               |
   +----------------------------------------------------------------------+
*/
#ifdef FRICC2_INPHP_COMPILE
	#include "php.h"
#else
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
#endif
#include <zlib.h>
#include "fricc2_lib.h"

void fricc2_lib_decrypt(char *file_buf, size_t *file_buf_len)
{
	int i;
	short friccload_key[] = {
		FRICCKEY
	};
	int cryptokey_len = sizeof friccload_key / 2;

	// start_rre
	i = friccload_key[13];
	friccload_key[13] = friccload_key[12];
	friccload_key[12] = i;
	i = friccload_key[9];
	friccload_key[9] = friccload_key[6];
	friccload_key[6] = i;
	i = friccload_key[7];
	friccload_key[7] = friccload_key[8];
	friccload_key[8] = i;
	i = friccload_key[1];
	friccload_key[1] = friccload_key[11];
	friccload_key[11] = i;
	// end_rre

   	for (i=0; i<*file_buf_len; i++)
	 	file_buf[i] = (char)friccload_key[(*file_buf_len - i) % cryptokey_len] ^ (~(file_buf[i]));

	return;
}

char *fricc2_lib_zcodecom(int mode, char *inbuf, size_t inbuf_len, size_t *resultbuf_len)
{
	z_stream z;
	char outbuf[OUTBUFSIZ];

	int count, status;
	char *resultbuf;
	size_t total_count = 0;
	
	z.zalloc = Z_NULL;
 	z.zfree = Z_NULL;
 	z.opaque = Z_NULL;

	z.next_in = Z_NULL;
	z.avail_in = 0;
 	if (mode == 0) {
 		deflateInit(&z, 1);
 	} else {
 		inflateInit(&z);
 	}

 	z.next_out = (Bytef *)outbuf;
 	z.avail_out = OUTBUFSIZ;
 	z.next_in = (Bytef *)inbuf;
 	z.avail_in = inbuf_len;

#ifdef FRICC2_INPHP_COMPILE
 	resultbuf = safe_emalloc(OUTBUFSIZ, sizeof(char), 0);
#else
	resultbuf = malloc(OUTBUFSIZ);
#endif 	
 	while (1) {
 	 	if (mode == 0) {
 	 		status = deflate(&z, Z_FINISH);
 	 	} else {
 	 		status = inflate(&z, Z_NO_FLUSH);
 	 	}
 	 	if (status == Z_STREAM_END) break;
 	 	if (status != Z_OK) {
 	 		if (mode == 0) {
 	 			deflateEnd(&z);
 	 		} else {
 	 			inflateEnd(&z);
 	 		}
 	 		*resultbuf_len = 0;
 	 		return(resultbuf);
 	 	}
 	 	if (z.avail_out == 0) {
#ifdef FRICC2_INPHP_COMPILE
 	 		resultbuf = safe_erealloc(resultbuf, total_count + OUTBUFSIZ+1, sizeof(char), 0); 	 		
#else
			resultbuf = realloc(resultbuf, total_count + OUTBUFSIZ);	
#endif
 	 		memcpy(resultbuf + total_count, outbuf, OUTBUFSIZ);
 	 		total_count += OUTBUFSIZ;
 	 		z.next_out = (Bytef *)outbuf;
 	 		z.avail_out = OUTBUFSIZ;
 	 	}
 	}

 	if ((count = OUTBUFSIZ - z.avail_out) != 0) {
#ifdef FRICC2_INPHP_COMPILE 	 	
 	 	resultbuf = safe_erealloc(resultbuf, total_count + OUTBUFSIZ+1, sizeof(char), 0);
#else
 	 	resultbuf = realloc(resultbuf, total_count + OUTBUFSIZ);
#endif
 	 	memcpy(resultbuf + total_count, outbuf, count);
 	 	total_count += count;
 	}
 	if (mode == 0) {
 		deflateEnd(&z);
 	} else {
 		inflateEnd(&z);
 	}
	
 	*resultbuf_len = total_count;
 	return(resultbuf);
}
