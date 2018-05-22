#include <stdio.h>
#include "coroutine.h"

void test(void *_c) {
  coroutine_t *c = _c;
  int i = 0;
  while (1) {
    printf("slave %d\n", i++);
    coroutine_yield(c);
  }
}

#define SIZE 0x1000

int main(void) {
  coroutine_t c;
  char stack[SIZE];
  coroutine_init(&c, (void *)(stack + SIZE), test, (void *)&c);
  for (int i = 0; i < 10; i++) {
    printf("master %d\n", i);
    coroutine_invoke(&c);
  }
  return 0;
}
