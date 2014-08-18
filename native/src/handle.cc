#include "handle.h"

using namespace native;
using namespace base;

void native::base::_delete_handle(uv_handle_t* h)
{
    assert(h);

    // clean up SCM
    if(h->data)
    {
        delete reinterpret_cast<callbacks*>(h->data);
        h->data = nullptr;
    }

    switch(h->type)
    {
        case UV_TCP: delete reinterpret_cast<uv_tcp_t*>(h); break;
        default: assert(0); break;
    }
}
