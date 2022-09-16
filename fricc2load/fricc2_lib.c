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

	// 每次压缩/解压缩的缓冲
	char outbuf[OUTBUFSIZ]; // 数据缓冲
	int count, status; // 数据缓冲量, 缓冲器状态

	// 总数据
	char *resultbuf = NULL, *resultbuf2 = NULL; // 已读取数据指针, 临时已读取数据指针
	size_t total_count = 0; // 已读取数据总量
	
	// 初始化zlib
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

	// 首次读取初始化
	z.next_out = (Bytef *)outbuf; // 目标缓冲器
	z.avail_out = OUTBUFSIZ; // 目标缓冲器大小
	z.next_in = (Bytef *)inbuf; // 原始数据
	z.avail_in = inbuf_len; // 原始数据总长度

#ifdef FRICC2_INPHP_COMPILE
	resultbuf = safe_emalloc(OUTBUFSIZ+1, sizeof(char), 0);
#else
	resultbuf = malloc(OUTBUFSIZ+1);
#endif
	if (resultbuf == NULL) { // 内存申请失败
		*resultbuf_len = 0;
		return(NULL);
	}

	// 每次读取
	while (1) {
		// 数据缓冲读取
		if (mode == 0) {
			status = deflate(&z, Z_FINISH);
		} else {
			status = inflate(&z, Z_NO_FLUSH);
		}
		// 如果读取完成, 则结束循环进入后半段处理
		if (status == Z_STREAM_END) break;
		// 如果读取出错, 直接结束程序
		if (status != Z_OK) {
			if (mode == 0) {
				deflateEnd(&z);
			} else {
				inflateEnd(&z);
			}
#ifdef FRICC2_INPHP_COMPILE
			efree(resultbuf);
#else
			free(resultbuf);
#endif
			*resultbuf_len = 0;
			return(NULL);
		}
		// 如果Z_OK并且目标缓冲器读满了，则保存一次
		// 备注：如果目标缓冲器没满很有可能会进入Z_STREAM_END
		if (z.avail_out == 0) {
#ifdef FRICC2_INPHP_COMPILE
			resultbuf2 = safe_erealloc(resultbuf, total_count+OUTBUFSIZ+1, sizeof(char), 0);
#else
			resultbuf2 = realloc(resultbuf, total_count+OUTBUFSIZ+1);
#endif
			if (resultbuf2 == NULL) { // 内存申请失败
				if (mode == 0) {
					deflateEnd(&z);
				} else {
					inflateEnd(&z);
				}
#ifdef FRICC2_INPHP_COMPILE
				efree(resultbuf);
#else
				free(resultbuf);
#endif
				*resultbuf_len = 0;
				return(NULL);
			}
			resultbuf = resultbuf2; // 内存申请成功

			// 保存
			memcpy(resultbuf+total_count, outbuf, OUTBUFSIZ);
			total_count += OUTBUFSIZ;
			z.next_out = (Bytef *)outbuf;
			z.avail_out = OUTBUFSIZ;
		}
	}

	// 后半段处理
	// 如果最后一次还有数据要读, 比如Z_STREAM_END过来就还有一些数据
	if ((count = OUTBUFSIZ - z.avail_out) != 0) {
#ifdef FRICC2_INPHP_COMPILE
		resultbuf2 = safe_erealloc(resultbuf, total_count+OUTBUFSIZ+1, sizeof(char), 0);
#else
		resultbuf2 = realloc(resultbuf, total_count+OUTBUFSIZ+1);
#endif
		if (resultbuf2 == NULL) { // 内存申请失败
			if (mode == 0) {
				deflateEnd(&z);
			} else {
				inflateEnd(&z);
			}
#ifdef FRICC2_INPHP_COMPILE
			efree(resultbuf);
#else
			free(resultbuf);
#endif
			*resultbuf_len = 0;
			return(NULL);
		}
		resultbuf = resultbuf2; // 内存申请成功

		// 保存
		memcpy(resultbuf+total_count, outbuf, count);
		total_count += count;
	}

	// 正常结束
	if (mode == 0) {
		deflateEnd(&z);
	} else {
		inflateEnd(&z);
	}

	*resultbuf_len = total_count;
	resultbuf[total_count] = '\0';
	return(resultbuf);
}
