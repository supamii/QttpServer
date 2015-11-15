#ifndef __NATIVE_BASE_H__
#define __NATIVE_BASE_H__

#include <cassert>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <functional>
#include <map>
#include <algorithm>
#include <list>
#include <set>
#include <tuple>

#include <uv.h>

#define NNATIVE_MAKE_VERSION(major, minor, patch)                                \
      ((major) * 0x1000 + (minor) * 0x100 + (patch))

#ifdef __clang__
# define NNATIVE_CLANG_AT_LEAST(major, minor, patch)                             \
      (NNATIVE_MAKE_VERSION(major, minor, patch) <=                                  \
             NNATIVE_MAKE_VERSION(__clang_major__, __clang_minor__, __clang_patchlevel__))
#else
# define NNATIVE_CLANG_AT_LEAST(major, minor, patch) (0)
#endif

#ifdef __GNUC__
# define NNATIVE_GNUC_AT_LEAST(major, minor, patch)                              \
      (NNATIVE_MAKE_VERSION(major, minor, patch) <=                                  \
             NNATIVE_MAKE_VERSION(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__))
#else
# define NNATIVE_GNUC_AT_LEAST(major, minor, patch) (0)
#endif

#if NNATIVE_CLANG_AT_LEAST(2, 9, 0) || NNATIVE_GNUC_AT_LEAST(4, 5, 0)
# define NNATIVE_DEPRECATED(message, declarator)                                 \
        __attribute__((deprecated(message))) declarator
#elif defined(_MSC_VER)
# define NNATIVE_DEPRECATED(message, declarator)                                 \
        __declspec(deprecated(message)) declarator
#else
# define NNATIVE_DEPRECATED(message, declarator)                                 \
        declarator
#endif

namespace native
{
    namespace internal
    {
        enum uv_callback_id
        {
            uv_cid_close = 0,
            uv_cid_listen,
            uv_cid_read_start,
            uv_cid_write,
            uv_cid_shutdown,
            uv_cid_connect,
            uv_cid_connect6,
            uv_cid_max
        };
    }

    // TODO to finish this class
    class BaseObject
    {

    };
}

#endif
