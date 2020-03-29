#include "smmem.h"
#include "debug.h"
#include "smprint.h"
#include <errno.h>
#include <unistd.h>

int free_list_idx(size_t blockSize) {
  int highestBit = 0;
  int currentBit = 0;
  while (blockSize > 0) {
    if ((blockSize & 1) == 1) {
      highestBit = currentBit;
    }
    currentBit++;
    blockSize >>= 1;
  }
  // if highestBit is 6, then it is 2^6. Anything smaller than that
  // should not be possible in exisitng blocks but can be requested
  // in smalloc. 2^6 starts at index 0. Anything
  // with a highest bit above 14 should be placed in the last list
  if (highestBit < 6) {
    return 0;
  } else if (highestBit > 14) {
    return 9;
  } else {
    return highestBit - 6;
  }
}

sblock free_lists[NUM_FREE_LISTS];
static int bud_isInitialized = 0;

static inline void init_check() {
  if (!bud_isInitialized) {
    for (int i = 0; i < NUM_FREE_LISTS; i++) {
      free_lists[i].body.links.next = &free_lists[i];
      free_lists[i].body.links.prev = &free_lists[i];
      bud_isInitialized = 1;
    }
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
  int listIdx = free_list_idx(BLOCK_SIZE(block));
  block->body.links.next = free_lists[listIdx].body.links.next;
  block->body.links.prev = &free_lists[listIdx];
  free_lists[listIdx].body.links.next = block;
  block->body.links.next->body.links.prev = block;
}

void fill_free_header(sblock *block) {
  block->header.header &= ~0xf;
  block->header.header |= (0x5 < 1);
  block->header.header &= ((~0) - 1);
}

void fill_alloc_header(sblock *block) {
  block->header.header &= ~0xf;
  block->header.header |= (0x5 < 1);
  block->header.header |= 1;
}

void block_split_to_size(size_t size, sblock *block) {
  // smallest legal size is 64 bytes. 128 can be split to 64 x 2 hence
  // BLOCK_SIZE(block) >= 128
  while (BLOCK_SIZE(block) >= (2 * size) && BLOCK_SIZE(block) >= 128) {
    size_t blockSize = BLOCK_SIZE(block);
    void *half = ((void *)block) + (blockSize / 2);
    setblocksize(block, blockSize / 2);
    setblocksize(half, blockSize / 2);
    fill_free_header(half);
    relink_block(half);
  }
}

int accumulate_system_memory(size_t minBytes, size_t *accumulated,
                             void **memStart) {
  *memStart = sbrk(0);
  if (*memStart == NULL) {
    errno = ENOMEM;
    return -1;
  }

  size_t acc = 0;
  while (acc < minBytes) {
    void *nextBlock = sbrk(getpagesize());
    if (nextBlock == NULL) {
      errno = ENOMEM;
      return -1;
    }
    acc += getpagesize();
  }
  *accumulated = acc;
  return 0;
}

sblock *search_free_list(int startIndex, size_t minBlockSize) {
  int listIndex = -1;
  sblock *candidate = NULL;
  size_t recordDifference = (size_t)-1;

  for (int i = startIndex; i < NUM_FREE_LISTS; i++) {
    sblock *crawler = free_lists[i].body.links.next;

    while (crawler != &free_lists[i]) {
      size_t blockSize = BLOCK_SIZE(crawler);
      size_t nsize = minBlockSize;
      if (blockSize >= nsize) {
        if (blockSize - nsize < recordDifference) {
          recordDifference = blockSize - nsize;
          candidate = crawler;
          listIndex = i;
        }
      }
      crawler = crawler->body.links.next;
    }
  }
  return candidate;
}

void *smalloc(size_t size) {
  init_check();

  if (size <= 0) {
    errno = EINVAL;
    return NULL;
  }

  size_t requiredBlockSize = NSIZE(size);

  int startIndex = free_list_idx(requiredBlockSize);
  sblock *candidate = search_free_list(startIndex, requiredBlockSize);

  if (candidate == NULL) {
    size_t accumulated;

    int result = accumulate_system_memory(requiredBlockSize, &accumulated,
                                          (void **)&candidate);
    if (result == -1) {
      errno = ENOMEM;
      return NULL;
    }

    setblocksize(candidate, accumulated);
  } else {
    // remove the candidate block from the free lists so it can be
    // split/allocated
    unlink_block(candidate);
  }

  block_split_to_size(requiredBlockSize, candidate);

  fill_alloc_header(candidate);

  return &candidate->body.payload;
}

void sfree(void *ptr) {
  init_check();

  if (ptr == NULL) {
    abort();
  }

  sblock *block = ptr - sizeof(sheader);

  // TODO: Coalesce blocks
  block->header.header &= ((~0) - 1); // unset alloc bit

  relink_block(block);
}

void *srealloc(void *ptr, size_t size) {
  init_check();
  error("%s\n", "NOT IMPLEMENTED");
  abort();
  return NULL;
}
