#include "uv.h"

#ifndef UNUSED_ARG
#define UNUSED_ARG(X) (void) X;
#endif

void uv__check_close(uv_check_t* handle)
{
  UNUSED_ARG(handle);
}
void uv__idle_close(uv_idle_t* handle)
{
  UNUSED_ARG(handle);
}
void uv__prepare_close(uv_prepare_t* handle)
{
  UNUSED_ARG(handle);
}
void uv__run_check(uv_loop_t* loop)
{
  UNUSED_ARG(loop);
}
void uv__run_idle(uv_loop_t* loop)
{
  UNUSED_ARG(loop);
}
void uv__run_prepare(uv_loop_t* loop)
{
  UNUSED_ARG(loop);
}
