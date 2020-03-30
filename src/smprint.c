#include "smprint.h"

void print_free_list() {
  for (int i = 0; i < NUM_FREE_LISTS; i++) {
    sblock *start = &(free_lists[i]);

    printf("Free list order: %d\n", i);
    printf("prev: %lx, sentinel: %p, next: %lx\n",
           LAST_HEXITS(free_lists[i].body.links.prev, 5), &(free_lists[i]),
           LAST_HEXITS(free_lists[i].body.links.next, 5));

    do {
      printf("prev: %lx, node: %p, next: %lx\n",
             LAST_HEXITS(start->body.links.prev, 5), start,
             LAST_HEXITS(start->body.links.next, 5));
      start = start->body.links.next;
    } while (start != &free_lists[i]);
  }
}

void print_free_list_blocks() {
  for (int i = 0; i < NUM_FREE_LISTS; i++) {
    printf("Free list order: %d\n", i);
    sblock *start = &(free_lists[i]);

    do {
      print_block(start);
      start = start->body.links.next;
    } while (start != &(free_lists[i]));
  }
}

void print_block(sblock *block) {
  for (int i = 0; i < 40; i++)
    printf("=+");
  printf("\n");
  printf("|Block at: %p\n", block);
  printf("| size: %lu | magic: %lx (ok? %d) | alloc: %d |\n", BLOCK_SIZE(block),
         MAGIC_GET(block), MAGIC_CHECK(block), IS_ALLOC(block));
  if (IS_FREE(block)) {
    printf("| prev: %p | next: %p |\n", block->body.links.prev,
           block->body.links.next);
  }
  for (int i = 0; i < 40; i++)
    printf("=+");
  printf("\n");
}

size_t free_list_total_size() {
  size_t total = 0;
  for (int i = 0; i < NUM_FREE_LISTS; i++) {
    sblock *start = &free_lists[i];

    do {
      total += BLOCK_SIZE(start);
      start = start->body.links.next;
    } while (start != &free_lists[i]);
  }
  return total;
}

size_t free_list_total_count() {
  size_t total = 0;
  for (int i = 0; i < NUM_FREE_LISTS; i++) {

    sblock *start = &free_lists[i];

    do {
      total++;
      start = start->body.links.next;
    } while (start != &free_lists[i]);
  }
  return total - NUM_FREE_LISTS; // every sentinel is counted once
}

void print_free_list_stats() {
  printf("=|=> Total free list size : %lu\n", free_list_total_size());
  printf(" '=> Total free list count: %lu\n", free_list_total_count());
}
