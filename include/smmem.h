
#ifndef BUDMM_H
#define BUDMM_H

#include <stdlib.h>

// header needs to be 32 bytes for alignment
typedef struct sheader {
  size_t header;   //  8
  void *prev_addr; // 16
  size_t padding2; // 24
  size_t padding3; // 32
} __attribute__((packed)) sheader;

#define UNSET_ALLOC_BIT(block) (((block)->header.header) &= ((~0) - 1))
#define IS_ALLOC(block) ((((block)->header.header) & 1) == 1)
#define IS_FREE(block) ((((block)->header.header) & 1) == 0)
#define BLOCK_SIZE(block) (((block)->header.header) & (~0xf))
#define MAGIC_CHECK(block) (((((block)->header.header) & 0xf)) == 0x5)
#define MAGIC_GET(block) ((((block)->header.header) & 0xf))

#define NSIZE(size) ((size) + sizeof(sheader))
#define LAST_HEXITS(n, amt) (((size_t)n) & ((1 << ((amt + 1) * 4)) - 1))

// payloads are 32 byte aligned but minimum block size is 64 bytes
// the header is 8 bytes of information 24 bytes of padding.
// this allows payload to be 32 byte aligned however since
// 1 + 32 = 33 the smallest 32 byte aligned block size possible is 64 bytes
/*
 * 60 bits for size, 3 magic (0x5 [0b101]), 1 bool allocated
 */
typedef struct sblock {
  sheader header;
  union {
    // free blocks
    struct {
      struct sblock *next;
      struct sblock *prev;
    } links;
    // alloc block
    void *payload;
  } body;
} sblock;

/*
 * Free lists should contain blocks of size 2^6, 2^7, 2^8...2^14 (pagesize),
 * 2^14+ This requires 10 free lists
 */

#define NUM_FREE_LISTS 10
extern sblock free_lists[NUM_FREE_LISTS];
void *heap_start;

void *smalloc(size_t size);
void sfree(void *ptr);
void *srealloc(void *ptr, size_t size);

#endif
