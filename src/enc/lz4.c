#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lz4/lz4hc.h"

int
lz4enc(
	void *_src
	, unsigned src_sz
	, void *_dst
	, unsigned *dst_sz
	, void *_ctx
)
{
	char *src = _src;
	char *dst = _dst;
	unsigned long result_sz = 0;
	
	extern int g_hlen; /* header length */
	memset(dst, 0, g_hlen);
	memcpy(dst, "LZ40", 4);
	dst[4] = (src_sz >> 24);
	dst[5] = (src_sz >> 16);
	dst[6] = (src_sz >>  8);
	dst[7] = (src_sz >>  0);

	result_sz = LZ4_compress_HC(src, dst + g_hlen, src_sz, *dst_sz, LZ4HC_CLEVEL_MAX);

	*dst_sz = result_sz + g_hlen;

	return 0;
}
