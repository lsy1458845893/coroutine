#ifndef COROUTINE_H
#define COROUTINE_H
/**
 * author : <lsy1458845893@qq.com>
 * licence: MIT
 * note: add option -U_FORTIFY_SOURCE if you use gcc compile this code
 */
#include <setjmp.h>

// type definition
typedef struct coroutine {
  jmp_buf reg;
  jmp_buf ret;
} coroutine_t;

// api declaration
static inline int coroutine_init(coroutine_t *c, void *stack, void (*func)(void *), void *dat);
static inline int coroutine_yield(coroutine_t *c);
static inline int coroutine_invoke(coroutine_t *c);

#pragma GCC push_options
#pragma GCC optimize("no-omit-frame-pointer")

// api definition
static int __coroutine_start__(coroutine_t *c, void (*func)(void *), void *dat) {
  if (!setjmp(c->reg)) longjmp(c->ret, 1);
  func(dat);
  while (1)
    if (setjmp(c->reg)) longjmp(c->ret, 2);
}

#define COROUTINE_REGISER(reg) register void *reg __asm__("" #reg);

static inline int coroutine_init(coroutine_t *c, void *stack, void (*func)(void *), void *dat) {
  COROUTINE_REGISER(sp)
  void *ssp = sp;
  if (!setjmp(c->ret)) {
    sp = stack;
    __coroutine_start__(c, func, dat);
  }
  sp = ssp;
}

static inline int coroutine_yield(coroutine_t *c) {
  if (!setjmp(c->reg)) longjmp(c->ret, 1);
}

static inline int coroutine_invoke(coroutine_t *c) {
  int ret;
  if (!(ret = setjmp(c->ret)))
    longjmp(c->reg, 1);
  else
    return ret == 1 ? 1 : 0;
}
#pragma GCC pop_options

#endif