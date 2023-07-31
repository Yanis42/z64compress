#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define LZ4_DISTANCE_MAX 16384
#include "./lz4/lz4hc.h"

static bool includeHeader = true;

int lz4enc(void *_src, unsigned src_sz, void *_dst, unsigned *dst_sz, void *_ctx) {
	char *src = _src;
	char *dst = _dst;
	int result_sz = 0;
	int compress_bound = 0;
	short lenHeader = 0;

	/* build header */
	if (includeHeader)
	{
		extern int g_hlen; /* header length */
		short lenHeader = g_hlen + 4 + 4;
		memset(dst, 0, lenHeader);
		memcpy(dst, "LZ40", 4);

		// save src_sz
		dst[4] = (src_sz >> 24);
		dst[5] = (src_sz >> 16);
		dst[6] = (src_sz >> 8);
		dst[7] = (src_sz >> 0);
	}

	/* compress */
	compress_bound = LZ4_COMPRESSBOUND(src_sz);
	result_sz = LZ4_compress_HC(src, dst + lenHeader, src_sz, compress_bound, LZ4HC_CLEVEL_MAX);
	assert(result_sz > 0);
	assert(result_sz <= compress_bound);

	/* save compressed size in the header and align it */
	if (includeHeader)
	{
		// save result_sz
		dst[8] = (result_sz >> 24);
		dst[9] = (result_sz >> 16);
		dst[10] = (result_sz >> 8);
		dst[11] = (result_sz >> 0);

		// align
		dst[12] = dst[13] = dst[14] = dst[15] = 0;
	}

	/* update the compressed size parameter*/
	*dst_sz = result_sz + lenHeader;
	return 0;
}
