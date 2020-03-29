#include "smprint.h"

void print_free_list() {
  sblock *start = &free_list;

  printf("prev: %lx, sentinel: %p, next: %lx\n",
         LAST_HEXITS(free_list.body.links.prev, 5), &free_list,
         LAST_HEXITS(free_list.body.links.next, 5));

  do {
    printf("prev: %lx, node: %p, next: %lx\n",
           LAST_HEXITS(start->body.links.prev, 5), start,
           LAST_HEXITS(start->body.links.next, 5));
    start = start->body.links.next;
  } while (start != &free_list);
}

void print_free_list_blocks() {
  sblock *start = &free_list;

  do {
    print_block(start);
    start = start->body.links.next;
  } while (start != &free_list);
}

void print_block(sblock *block) {
  for (int i = 0; i < 40; i++)
    printf("=+");
  printf("\n");
  printf("|Block at: %p\n", block);
  printf("| size: %lu | magic: %x (ok? %d) | alloc: %d |\n", BLOCK_SIZE(block),
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
  sblock *start = &free_list;

  do {
    total += BLOCK_SIZE(start);
    start = start->body.links.next;
  } while (start != &free_list);

  return total;
}

size_t free_list_total_count() {
  size_t total = 0;
  sblock *start = &free_list;

  do {
    total++;
    start = start->body.links.next;
  } while (start != &free_list);

  return total;
}

void print_free_list_stats() {
  printf("=|=> Total free list size : %lu\n", free_list_total_size());
  printf(" '=> Total free list count: %lu\n", free_list_total_count());
}
