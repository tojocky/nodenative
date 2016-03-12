#ifndef __NATIVE_WORKER_WORKERCALLBACK_HPP__
#define __NATIVE_WORKER_WORKERCALLBACK_HPP__

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
 * Propose :
 * -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

#include "../helper/TemplateSeqInd.hpp"
#include "../async/FutureError.hpp"
#include "WorkerBase.hpp"
#include "../async/FutureSharedResolver.hpp"

namespace native {

template<class R>
class FutureShared;

template<typename R>
class Future;

/** Acction callback base class template specialization for void type.
 */
class WorkerCallbackBase : public std::enable_shared_from_this<WorkerCallbackBase> {
protected:
    WorkerCallbackBase() {}

public:
    virtual ~WorkerCallbackBase() {}

    void resolve();

    virtual void resolveCb() = 0;
    virtual void executeWorkerAfter(int iStatus) = 0;
    virtual std::shared_ptr<Loop> getLoop() = 0;
};

class WorkerCallbackBaseDetached : public WorkerBase {
private:
    std::shared_ptr<WorkerCallbackBase> _instance;
    WorkerCallbackBaseDetached(std::shared_ptr<WorkerCallbackBase> iInstance);

public:
    WorkerCallbackBaseDetached() = delete;
    ~WorkerCallbackBaseDetached() {
        NNATIVE_FCALL();
    }

    void executeWorker() override;
    void executeWorkerAfter(int iStatus) override;

    static void Enqueue(std::shared_ptr<WorkerCallbackBase> iInstance);
};

template<typename R, typename... Args>
class WorkerCallback: public WorkerCallbackBase {
    std::function<R(Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<R>> _future;
    std::unique_ptr<FutureSharedResolver<R>> _resolver;

    template<std::size_t... Is>
    void callFn(helper::TemplateSeqInd<Is...>);

    void resolveCb() override;
    WorkerCallback(std::shared_ptr<Loop> iLoop, std::function<R(Args...)> f, Args&&... args);

public:
    typedef R ResultType;

    WorkerCallback() = delete;
    static std::shared_ptr<WorkerCallback<R, Args...>> Create(std::shared_ptr<Loop> iLoop, std::function<R(Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<R>> getFuture();
    std::shared_ptr<Loop> getLoop() override;
    void executeWorkerAfter(int iStatus) override;
};

template<typename R, typename... Args>
class WorkerCallback<Future<R>, Args...>: public WorkerCallbackBase {
    std::function<Future<R>(Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<R>> _future;
    std::unique_ptr<FutureSharedResolver<R>> _resolver;

    template<std::size_t... Is>
    void callFn(helper::TemplateSeqInd<Is...>);

    void resolveCb() override;

    WorkerCallback(std::shared_ptr<Loop> iLoop, std::function<Future<R>(Args...)> f, Args&&... args);

public:
    typedef R ResultType;

    WorkerCallback() = delete;
    static std::shared_ptr<WorkerCallback<Future<R>, Args...>> Create(std::shared_ptr<Loop> iLoop, std::function<Future<R>(Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<R>> getFuture();
    std::shared_ptr<Loop> getLoop() override;
    void executeWorkerAfter(int iStatus) override;
};

template<typename... Args>
class WorkerCallback<Future<void>, Args...>: public WorkerCallbackBase {
    std::function<Future<void>(Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<void>> _future;
    std::unique_ptr<FutureSharedResolver<void>> _resolver;

    template<std::size_t... Is>
    void callFn(helper::TemplateSeqInd<Is...>);

    void resolveCb() override;

    WorkerCallback(std::shared_ptr<Loop> iLoop, std::function<Future<void>(Args...)> f, Args&&... args);

public:
    typedef void ResultType;

    WorkerCallback() = delete;
    static std::shared_ptr<WorkerCallback<Future<void>, Args...>> Create(std::shared_ptr<Loop> iLoop, std::function<Future<void>(Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<void>> getFuture();
    std::shared_ptr<Loop> getLoop() override;
    void executeWorkerAfter(int iStatus) override;
};

template<typename... Args>
class WorkerCallback<void, Args...>: public WorkerCallbackBase {
    std::function<void(Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<void>> _future;
    std::unique_ptr<FutureSharedResolver<void>> _resolver;

    template<std::size_t... Is>
    void callFn(helper::TemplateSeqInd<Is...>);

    void resolveCb() override;

    WorkerCallback(std::shared_ptr<Loop> iLoop, std::function<void(Args...)> f, Args&&... args);

public:
    typedef void ResultType;

    WorkerCallback() = delete;
    static std::shared_ptr<WorkerCallback<void, Args...>> Create(std::shared_ptr<Loop> iLoop, std::function<void(Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<void>> getFuture();
    std::shared_ptr<Loop> getLoop() override;
    void executeWorkerAfter(int iStatus) override;
};

} /* namespace native */

#endif /* __NATIVE_WORKER_WORKERCALLBACK_HPP__ */
