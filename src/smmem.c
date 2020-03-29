#include "smmem.h"
#include "debug.h"
#include "smprint.h"
#include <errno.h>
#include <unistd.h>

sblock free_list;
static int bud_isInitialized = 0;

static inline void init_check() {
  if (!bud_isInitialized) {
    free_list.body.links.next = &free_list;
    free_list.body.links.prev = &free_list;
    bud_isInitialized = 1;
  }
}

void setblocksize(sblock *block, size_t size) {
  block->header.header = block->header.header & 0xf;
  block->header.header |= size;
}

void unlink_block(sblock *block) {
  sblock *previous = block->body.links.prev;
  sblock *next = block->body.links.next;

  previous->body.links.next = next;
  next->body.links.prev = previous;
}

void relink_block(sblock *block) {
  block->body.links.next = free_list.body.links.next;
  block->body.links.prev = &free_list;
  free_list.body.links.next = block;
  block->body.links.next->body.links.prev = block;
}

void fill_free_header(sblock *block_start) {
  block_start->header.header &= ~0xf;
  block_start->header.header |= (0x5 < 1);
}

void fill_alloc_header(sblock *block_start) {
  block_start->header.header &= ~0xf;
  block_start->header.header |= (0x5 < 1);
  block_start->header.header |= 1;
}

void *smalloc(size_t size) {
  init_check();
  if (size <= 0) {
    errno = EINVAL;
    return NULL;
  }

  sblock *crawler = free_list.body.links.next;
  sblock *candidate = NULL;

#ifdef FIRST_FIT

  while (crawler != &free_list) {
    if (BLOCK_SIZE(crawler) >= NSIZE(size)) {
      candidate = crawler;
      break;
    }
    crawler = crawler->body.links.next;
  }

#else

  size_t recordDifference = (size_t)-1;
  while (crawler != &free_list) {
    size_t blockSize = BLOCK_SIZE(crawler);
    size_t nsize = NSIZE(size);
    if (blockSize >= nsize) {
      if (blockSize - nsize < recordDifference) {
        recordDifference = blockSize - nsize;
        candidate = crawler;
      }
    }
    crawler = crawler->body.links.next;
  }

#endif

  if (candidate == NULL) {
    size_t requiredBlockSize = NSIZE(size);
    size_t accumulated = 0;

    candidate = sbrk(0);
    if (candidate == NULL) {
      errno = ENOMEM;
      return NULL;
    }

    while (accumulated < requiredBlockSize) {
      void *nextBlock = sbrk(getpagesize());
      if (nextBlock == NULL) {
        errno = ENOMEM;
        return NULL;
      }
      accumulated += getpagesize();
    }

    setblocksize(candidate, accumulated);
  } else {
    // remove the candidate block from the free lists so it can be
    // split/allocated
    unlink_block(candidate);
  }

  while (BLOCK_SIZE(candidate) >= 2 * NSIZE(size) &&
         BLOCK_SIZE(candidate) >= 128) {
    size_t blockSize = BLOCK_SIZE(candidate);
    void *half = ((void *)candidate) + (blockSize / 2);
    setblocksize(candidate, blockSize / 2);
    setblocksize(half, blockSize / 2);
    fill_free_header(half);
    relink_block(half);
  }

  fill_alloc_header(candidate);

  return &candidate->body.payload;
}

void sfree(void *ptr) {
  init_check();

  if (ptr == NULL) {
    abort();
  }

  sblock *block = ptr - sizeof(sheader);

  block->header.header &= ((~0) - 1); // unset alloc bit

  relink_block(block);
}
void *srealloc(void *ptr, size_t size) {
  init_check();
  error("%s\n", "NOT IMPLEMENTED");
  abort();
  return NULL;
}
