#ifndef __LOOP_H__
#define __LOOP_H__

#include "base.h"
#include "error.h"

namespace native
{
    /*!
     *  Class that represents the loop instance.
     */
    class loop
    {
    public:
        /*!
         *  Default constructor
         *  @param use_default indicates whether to use default loop or create a new loop.
         */
        loop(bool use_default=false)
            : uv_loop_(use_default ? uv_default_loop() : uv_loop_new())
        { }

        /*!
         *  Destructor
         */
        ~loop()
        {
            if(uv_loop_)
            {
                uv_loop_delete(uv_loop_);
                uv_loop_ = nullptr;
            }
        }

        /*!
         *  Returns internal handle for libuv functions.
         */
        uv_loop_t* get() { return uv_loop_; }

        /*!
         *  Runs the event loop until the reference count drops to zero. Always returns zero.
         *  Internally, this function just calls uv_run() function.
         */
        bool run() { 
            return uv_run(uv_loop_, UV_RUN_DEFAULT)==0; 
        }

        /*!
         *  Poll for new events once. Note that this function blocks if there are no pending events. Returns true when done (no active handles
         *  or requests left), or non-zero if more events are expected (meaning you
         *  should run the event loop again sometime in the future).
         *  Internally, this function just calls uv_run_once() function.
         */
        bool run_once() { 
            return uv_run(uv_loop_, UV_RUN_ONCE)==0; 
        }

        /*!
         *  Poll for new events once but don't block if there are no pending events.
         *  Internally, this function just calls uv_run_once() function.
         */
        bool run_nowait() { 
            return uv_run(uv_loop_, UV_RUN_NOWAIT)==0; 
        }

        /*!
         *  ...
         *  Internally, this function just calls uv_update_time() function.
         */
        void update_time() { uv_update_time(uv_loop_); }

        /*!
         *  ...
         *  Internally, this function just calls uv_now() function.
         */
        int64_t now() { return uv_now(uv_loop_); }

    private:
        loop(const loop&);
        void operator =(const loop&);

    private:
        uv_loop_t* uv_loop_;
    };

    /*!
     *  Starts the default loop.
     */
    bool run()
    {
        return (uv_run(uv_default_loop(), UV_RUN_DEFAULT) ==0);
    }

    /*!
     *  Polls for new events once for the default loop.
     *  Note that this function blocks if there are no pending events. Returns true when done (no active handles
     *  or requests left), or non-zero if more events are expected (meaning you
     *  should run the event loop again sometime in the future).
     */
    bool run_once()
    {
        return (uv_run(uv_default_loop(), UV_RUN_ONCE) == 0);
    }

    /*!
     *  Polls for new events once but don't block if there are no pending events for the default loop.
     */
    bool run_nowait()
    {
        return (uv_run(uv_default_loop(), UV_RUN_NOWAIT) ==0);
    }
}


#endif
