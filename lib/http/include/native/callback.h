#ifndef __CALLBACK_H__
#define __CALLBACK_H__

#include "base.h"

namespace native
{
namespace internal
{
class callback_object_base
{
  public:
    callback_object_base(void* data)
      : data_(data)
    {
    }
    virtual ~callback_object_base()
    {
    }

    void* get_data() {
      return data_;
    }

  private:
    void* data_;
};

// SCO: serialized callback object
template<typename callback_t>
class callback_object : public callback_object_base
{
  public:
    callback_object(const callback_t& callback, void* data = nullptr)
      : callback_object_base(data)
      , callback_(callback)
    {
    }

    virtual ~callback_object()
    {
    }

    template<typename ... A>
    typename std::result_of<callback_t(A ...)>::type invoke(A&& ... args)
    {
      return callback_(std::forward<A>(args) ...);
    }

  private:
    callback_t callback_;
};
}

typedef std::shared_ptr<internal::callback_object_base> callback_object_ptr;

class callbacks
{
  public:
    callbacks(int max_callbacks)
      : lut_(max_callbacks)
    {
    }
    ~callbacks()
    {
    }

    template<typename callback_t>
    static void store(void* target, int cid, const callback_t& callback, void* data = nullptr)
    {
      //TODO: remove new operator to pass directly as parameter. May cause memory leak
      std::vector<callback_object_ptr> & list = reinterpret_cast<callbacks*>(target)->lut_;
      list[cid] = callback_object_ptr(new internal::callback_object<callback_t>(callback, data));
      PRINT_DBG("Store: " << cid << " Address: " << std::hex << list[cid]);
    }

    template<typename callback_t>
    static void* get_data(void* target, int cid)
    {
      return reinterpret_cast<callbacks*>(target)->lut_[cid]->get_data();
    }

    template<typename callback_t, typename ... A>
    static typename std::result_of<callback_t(A ...)>::type invoke(void* target, int cid, A&& ... args)
    {
      auto y = reinterpret_cast<callbacks*>(target)->lut_[cid].get();
      auto x = dynamic_cast<internal::callback_object<callback_t>*>(y);
      PRINT_DBG("Invoke: " << cid << " Address: " << std::hex << y);
      assert(x);
      return x->invoke(std::forward<A>(args) ...);
    }

  private:
    std::vector<callback_object_ptr> lut_;
};
}

#endif
