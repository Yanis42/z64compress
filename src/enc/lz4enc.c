#include <string.h>

#define LZ4_DISTANCE_MAX 16384
#include "./lz4/lz4hc.h"

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
	int compress_bound = 0;
	
	extern int g_hlen; /* header length */
	short lenHeader = g_hlen + 4;
	memset(dst, 0, lenHeader);
	memcpy(dst, "LZ40", 4);

	// save src_sz
	dst[4] = (src_sz >> 24);
	dst[5] = (src_sz >> 16);
	dst[6] = (src_sz >>  8);
	dst[7] = (src_sz >>  0);

	compress_bound = LZ4_COMPRESSBOUND(src_sz);
	result_sz = LZ4_compress_HC(src, dst + lenHeader, src_sz, compress_bound, LZ4HC_CLEVEL_MAX);

	if (!result_sz || !(result_sz <= compress_bound))
		return 1;

	// save result_sz
	dst[8] 	= (result_sz >> 24);
	dst[9] 	= (result_sz >> 16);
	dst[10] = (result_sz >>  8);
	dst[11] = (result_sz >>  0);

	*dst_sz = result_sz + lenHeader;

	return 0;
}
