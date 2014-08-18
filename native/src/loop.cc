#include "loop.h"

using namespace native;


loop::loop(bool use_default) :
    uv_loop_(use_default ? uv_default_loop() : uv_loop_new())
{
}

loop::~loop()
{
    if(uv_loop_)
    {
        uv_loop_delete(uv_loop_);
        uv_loop_ = nullptr;
    }
}

bool loop::run() { 
    return (uv_run(uv_loop_, UV_RUN_DEFAULT) == 0); 
}

bool loop::run_once() { 
    return (uv_run(uv_loop_, UV_RUN_ONCE) == 0); 
}

void loop::update_time()
{
    uv_update_time(uv_loop_);
}

int64_t loop::now()
{
    return uv_now(uv_loop_);
}

bool native::run()
{
    return (uv_run(uv_default_loop(), UV_RUN_DEFAULT) == 0);
}

bool native::run_once()
{
    return (uv_run(uv_default_loop(), UV_RUN_ONCE) == 0);
}

bool native::run_nowait()
{
    return (uv_run(uv_default_loop(), UV_RUN_NOWAIT) == 0);
}
