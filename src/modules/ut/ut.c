#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>
#include "lg.h"
#include "ut.h"

LG_context_t *ut_context;

void
UT_init (LG_context_t *ctx)
{
  ut_context = ctx;
}

