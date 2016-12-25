#ifndef _COMPRESSED_LOOP_H
#define _COMPRESSED_LOOP_H

/*************************************************************************\
* Starting with Format V4.0 (cloop version 4.x), cloop can now have two   *
* alternative structures:                                                 *
*                                                                         *
* 1. Header first: "robust" format, handles missing blocks well           *
* 2. Footer (header last): "streaming" format, easier to create           *
*                                                                         *
* The cloop kernel module autodetects both formats, and can (currently)   *
* still handle the V2.0 format as well.                                   *
*                                                                         *
* 1. Header first:                                                        *
*   +---------------------------- FIXED SIZE ---------------------------+ *
*   |Signature (128 bytes)                                              | *
*   |block_size (32bit number, network order)                           | *
*   |num_blocks (32bit number, network order)                           | *
*   +--------------------------- VARIABLE SIZE -------------------------+ *
*   |num_blocks * FlagsOffset (upper 4 bits flags, lower 64 bits offset)| *
*   |compressed data blocks of variable size ...                        | *
*   +-------------------------------------------------------------------+ *
*                                                                         *
* 2. Footer (header last):                                                *
*   +--------------------------- VARIABLE SIZE -------------------------+ *
*   |compressed data blocks of variable size ...                        | *
*   |num_blocks * FlagsOffset (upper 4 bits flags, lower 64 bits offset)| *
*   +---------------------------- FIXED SIZE ---------------------------+ *
*   |Signature (128 bytes)                                              | *
*   |block_size (32bit number, network order)                           | *
*   |num_blocks (32bit number, network order)                           | *
*   +-------------------------------------------------------------------+ *
*                                                                         *
* Offsets are always relative to beginning of file, in all formats.       *
* The block index contains num_blocks+1 offsets, followed (1) or          *
* preceded (2) by the compressed blocks.                                  *
\*************************************************************************/

#include <linux/types.h>   /* u_int32_t */

#define CLOOP_HEADROOM 128

/* Header of fixed length, can be located at beginning or end of file   */
struct cloop_head
{
	char preamble[CLOOP_HEADROOM];
	u_int32_t block_size;
	u_int32_t num_blocks;
};

#define CLOOP2_SIGNATURE "V2.0"                       /* @ offset 0x0b  */
#define CLOOP2_SIGNATURE_SIZE 4
#define CLOOP2_SIGNATURE_OFFSET 0x0b
#define CLOOP4_SIGNATURE "V4.0"                       /* @ offset 0x0b  */
#define CLOOP4_SIGNATURE_SIZE 4
#define CLOOP4_SIGNATURE_OFFSET 0x0b

/************************************************************************\
*  CLOOP4 flags for each compressed block                                *
*  Value   Meaning                                                       *
*    0     GZIP/7ZIP compression (compatible with V2.0 Format)           *
*    1     no compression (incompressible data)                          *
*    2     xz compression (currently best space saver)                   *
*    3     lz4 compression                                               *
*    4     lzo compression (fastest)                                     *
\************************************************************************/

typedef uint64_t cloop_block_ptr;

/* Get value of first 4 bits */
#define CLOOP_BLOCK_FLAGS(x)  ((unsigned int)(((x) & 0xf000000000000000LLU) >> 60))
/* Get value of last 60 bits */
#define CLOOP_BLOCK_OFFSET(x)  ((x) & 0x0fffffffffffffffLLU)

#define CLOOP_COMPRESSOR_ZLIB  0x0
#define CLOOP_COMPRESSOR_NONE  0x1
#define CLOOP_COMPRESSOR_XZ    0x2
#define CLOOP_COMPRESSOR_LZ4   0x3
#define CLOOP_COMPRESSOR_LZO1X 0x4

#define CLOOP_COMPRESSOR_VALID(x) ((x) >= CLOOP_COMPRESSOR_ZLIB && (x) <= CLOOP_COMPRESSOR_LZO1X)

/* Cloop suspend IOCTL */
#define CLOOP_SUSPEND 0x4C07

#endif /*_COMPRESSED_LOOP_H*/
