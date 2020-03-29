#include "smmem.h"
#include "smprint.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

void general_test() {
  int i;

  char *ptr0 = (char *)smalloc(3);
  char *ptr1 = (char *)smalloc(10);

  printf("ptr0: %p, ptr1: %p\n", ptr0, ptr1);

  ptr1[0] = 'a';
  ptr1[1] = 'b';
  ptr1[2] = 'c';
  ptr1[3] = 'd';
  ptr1[4] = 'e';
  ptr1[5] = 'f';
  ptr1[6] = '\0';
  printf("ptr1 string: %s\n", ptr1);

  sfree(ptr0);
  sfree(ptr1);

  int *ptr2 = smalloc(sizeof(int) * 100);
  for (i = 0; i < 100; i++)
    ptr2[i] = i;

  void *ptr3 = smalloc(3000);
  printf("ptr3: %p\n", ptr3);

  sfree(ptr2);
  sfree(ptr3);

  ptr1 = smalloc(200);
  ptr3 = smalloc(20000);

  sfree(ptr1);
  sfree(ptr3);

  // print_free_list_blocks();
}

int main(int argc, char const *argv[]) {

  // this should always return the same freed block
  for (int i = 0; i < 1000; i++) {
    int *p = smalloc(sizeof(int) * 104);
    sfree(p);
  }

  printf("Done same block testing\n");
  print_free_list_stats();

  printf("Free list\n");
  print_free_list_blocks();
  return EXIT_SUCCESS;
}
