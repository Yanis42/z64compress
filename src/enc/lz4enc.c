/* adapted from libdragon */

#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

#define LZ4_DISTANCE_MAX 16384
#include "lz4/lz4hc.h"


int lz4enc(void *_src, unsigned src_sz, void *_dst, unsigned *dst_sz, void *_ctx) {
	char *src = _src;
	char *dst = _dst;
	int result_sz = 0;
	extern int g_hlen; /* header length */

	/* build header */
	memset(dst, 0, g_hlen);
	memcpy(dst, "LZ40", 4);

	// save decompressed size
	dst[4] = (src_sz >> 24);
	dst[5] = (src_sz >> 16);
	dst[6] = (src_sz >> 8);
	dst[7] = (src_sz >> 0);

	/* compress */
	int compress_bound = LZ4_COMPRESSBOUND(src_sz) - g_hlen;
	dst += g_hlen;

	if (!compress_bound) {
		fprintf(stderr, "error with compression bound\n");
		return 1;
	}

	result_sz = LZ4_compress_HC(src, dst, src_sz, compress_bound, LZ4HC_CLEVEL_MAX);
	if (!result_sz || !(result_sz <= compress_bound)) {
		fprintf(stderr, "error with result (0x%08X, 0x%08X)\n", result_sz, compress_bound);
		return 1;
	}

	/* update the compressed size parameter */
	*dst_sz = result_sz + g_hlen;
	return 0;
}
