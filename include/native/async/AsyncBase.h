#ifndef __NATIVE_ASYNC_ASYNCBASE_H__
#define __NATIVE_ASYNC_ASYNCBASE_H__

#include <uv.h>
#include "../loop.h"
#include "../helper/trace.h"

#include <memory>

namespace native {

class AsyncBase {
    std::shared_ptr<AsyncBase> _self;
    std::shared_ptr<uv_loop_t> _loop;

    // This method will be called from uv
    static void Async(uv_async_t*);
    static void AsyncClosed(uv_handle_t*);
protected:
    uv_async_t _uv_async;

    /** Public method called at event loop
     * Extend this class for custom usage
     */
    virtual void executeAsync() = 0;
    virtual void closeAsync(std::shared_ptr<AsyncBase>) {}
    void enqueue();
    AsyncBase() = delete;
    AsyncBase(loop &iLoop);
    AsyncBase(std::shared_ptr<uv_loop_t> iLoop);
public:
    template<class Child, typename... Args>
    static Child* Create(loop& iLoop, Args&&... args);
    virtual ~AsyncBase();
};

} /* namespace native */

#endif /* __NATIVE_ASYNC_ASYNCBASE_H__ */

