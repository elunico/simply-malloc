#include "smmem.h"
#include "smprint.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

void general_test() {
  int i;

  char *ptr0 = (char *)smalloc(3);
  char *ptr1 = (char *)smalloc(10);

  // FIXME: you cannot call library / external functions that call malloc, sbrk,
  // mmap etc. while using smalloc if you want coalescing enabled. Calling those
  // functions messes with the break pointer making it impossible to walk the
  // heap the reliably
  // printf("ptr0: %p, ptr1: %p\n", ptr0, ptr1);

  ptr1[0] = 'a';
  ptr1[1] = 'b';
  ptr1[2] = 'c';
  ptr1[3] = 'd';
  ptr1[4] = 'e';
  ptr1[5] = 'f';
  ptr1[6] = '\0';
  // printf("ptr1 string: %s\n", ptr1);

  sfree(ptr0);
  sfree(ptr1);

  int *ptr2 = smalloc(sizeof(int) * 100);
  for (i = 0; i < 100; i++)
    ptr2[i] = i;

  void *ptr3 = smalloc(3000);
  // printf("ptr3: %p\n", ptr3);

  sfree(ptr2);
  sfree(ptr3);

  ptr1 = smalloc(200);
  ptr3 = smalloc(20000);

  sfree(ptr1);
  sfree(ptr3);

  // print_free_list_blocks();
}

void realloc_test() {
  char *start = smalloc(10);

  for (int i = 0; i < 10; i++) {
    start[i] = (char)('a' + i);
  }
  start[9] = (char)0;

  char *next = srealloc(start, 20);
  assert(start == next);
  for (int i = 0; i < 20; i++) {
    next[i] = (char)('a' + i);
  }
  next[19] = (char)0;
  // FIXME: if you call free with some unfreed block you get a segfault so..
  // thats a problem
  // pretty sure that this is a walk_coalesce_free() issue
  // see issue #1

  // sfree(next);
}

int main(int argc, char const *argv[]) {

  realloc_test();

  general_test();

  // // // this should always return the same freed block
  // for (int i = 0; i < 1000; i++) {
  //   int *p = smalloc(sizeof(int) * 104);
  //   sfree(p);
  // }

  // printf("Done same block testing\n");

  printf("Free list\n");
  print_free_list_blocks();
  printf("Free list stats\n");
  print_free_list_stats();
  return EXIT_SUCCESS;
}
