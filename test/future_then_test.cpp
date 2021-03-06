#include "native/native.hpp"
#include "gtest/gtest.h"

#include <thread>

TEST(FutureThenTest, SetValueOnDifferentThread) {
    std::shared_ptr<native::Loop> currLoop = native::Loop::Create(true);
    native::Promise<void> promise(currLoop);

    bool calledOtherThread = false;
    bool promiseResolved = false;
    bool calledPromise = false;

    std::thread::id mainThreadId = std::this_thread::get_id();

    promise.getFuture().then([&calledPromise, &calledOtherThread, &promiseResolved, &mainThreadId](){
        std::thread::id currThreadId = std::this_thread::get_id();
        EXPECT_EQ(mainThreadId, currThreadId);

        EXPECT_EQ(calledOtherThread, true);
        EXPECT_EQ(promiseResolved, true);
        calledPromise = true;
    });

    native::async([&promise, &calledOtherThread, &mainThreadId](){
		return native::worker([&promise, &calledOtherThread, &mainThreadId](){
			calledOtherThread = true;

			std::thread::id currThreadId = std::this_thread::get_id();
			EXPECT_NE(mainThreadId, currThreadId);
			EXPECT_ANY_THROW(promise.resolve());
		});
    }).then([&promise, &promiseResolved, &mainThreadId](){
        promiseResolved = true;

        std::thread::id currThreadId = std::this_thread::get_id();
        EXPECT_EQ(mainThreadId, currThreadId);
        promise.resolve();
    });

    EXPECT_EQ(calledOtherThread, false);
    EXPECT_EQ(promiseResolved, false);
    EXPECT_EQ(calledPromise, false);

    currLoop->run();

    EXPECT_EQ(calledOtherThread, true);
    EXPECT_EQ(promiseResolved, true);
    EXPECT_EQ(calledPromise, true);
}

TEST(FutureThenTest, order)
{
    std::string order;
    std::string expectedOrder;
    std::shared_ptr<native::Loop> currLoop = native::Loop::Create(true);
    std::thread::id mainThreadId = std::this_thread::get_id();
    //{
        native::Promise<void> p(currLoop);
        native::Future<void> f1 = p.getFuture().then([&order, &mainThreadId]{
            order+="1";
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
        });

        native::Future<void> f2 = f1.then([&order, &mainThreadId]{
            order+=",2";
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
        });

        native::Future<void> f3 = f2.then([&order, &mainThreadId]{
            order+=",3";
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
        });

        native::Future<void> f4 = f3.then([&order, &mainThreadId]{
            order+=",4";
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
        });

        // Add more async work to second future
        native::Future<void> f21 = f1.then([&order, &mainThreadId]{
            order+=",21";
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
        });

        native::Future<void> f22 = f1.then([&order, &mainThreadId]{
            order+=",22";
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
        });

        native::Future<void> f23 = f1.then([&order, &mainThreadId]{
            order+=",23";
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
        });

        EXPECT_EQ(expectedOrder, order);

        p.resolve();

        EXPECT_EQ(expectedOrder, order);
    //}

    EXPECT_EQ(expectedOrder, order);

    currLoop->run();

    expectedOrder = "1,2,21,22,23,3,4";
    EXPECT_EQ(expectedOrder, order);
}

TEST(FutureThenTest, ReturnFuture)
{
    bool called_p1 = false;
    bool called_p2 = false;
    bool called_p1_after = false;
    std::shared_ptr<native::Loop> currLoop = native::Loop::Create(true);
    std::thread::id mainThreadId = std::this_thread::get_id();
    {
        native::Promise<void> promise(currLoop);
        promise.getFuture().then([&called_p1, &called_p2, &mainThreadId]() -> native::Future<void>{
            auto future = native::async([&called_p1, &called_p2, &mainThreadId](){
                EXPECT_EQ(called_p1, true);
                called_p2 = true;
                std::thread::id currThreadId = std::this_thread::get_id();
                EXPECT_EQ(mainThreadId, currThreadId);
            });
            called_p1 = true;
            EXPECT_EQ(called_p2, false);
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
            return future;
        }).then([&called_p1, &called_p2, &called_p1_after, &mainThreadId](){
            called_p1_after = true;
            EXPECT_EQ(called_p1, true);
            EXPECT_EQ(called_p2, true);
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
        });

        EXPECT_EQ(called_p1, false);
        EXPECT_EQ(called_p2, false);
        EXPECT_EQ(called_p1_after, false);

        promise.resolve();

        EXPECT_EQ(called_p1, false);
        EXPECT_EQ(called_p2, false);
        EXPECT_EQ(called_p1_after, false);
    }

    EXPECT_EQ(called_p1, false);
    EXPECT_EQ(called_p2, false);
    EXPECT_EQ(called_p1_after, false);

    currLoop->run();

    EXPECT_EQ(called_p1, true);
    EXPECT_EQ(called_p2, true);
    EXPECT_EQ(called_p1_after, true);
}

TEST(FutureThenTest, ReturnFutureWithValue)
{
    bool called_p1 = false;
    bool called_p2 = false;
    bool called_p1_after = false;
    std::shared_ptr<native::Loop> currLoop = native::Loop::Create(true);
    std::thread::id mainThreadId = std::this_thread::get_id();
    {
        native::Promise<void> promise(currLoop);
        promise.getFuture().then([&called_p1, &called_p2, &mainThreadId]() -> native::Future<int>{
            auto future = native::async([&called_p1, &called_p2, &mainThreadId]() -> int{
                EXPECT_EQ(called_p1, true);
                called_p2 = true;
                std::thread::id currThreadId = std::this_thread::get_id();
                EXPECT_EQ(mainThreadId, currThreadId);
                return 1;
            });
            called_p1 = true;
            EXPECT_EQ(called_p2, false);
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
            return future;
        }).then([&called_p1, &called_p2, &called_p1_after, &mainThreadId](int iValue){
            called_p1_after = true;
            EXPECT_EQ(iValue, 1);
            EXPECT_EQ(called_p1, true);
            EXPECT_EQ(called_p2, true);
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
        });

        EXPECT_EQ(called_p1, false);
        EXPECT_EQ(called_p2, false);
        EXPECT_EQ(called_p1_after, false);

        promise.resolve();

        EXPECT_EQ(called_p1, false);
        EXPECT_EQ(called_p2, false);
        EXPECT_EQ(called_p1_after, false);
    }

    EXPECT_EQ(called_p1, false);
    EXPECT_EQ(called_p2, false);
    EXPECT_EQ(called_p1_after, false);

    currLoop->run();

    EXPECT_EQ(called_p1, true);
    EXPECT_EQ(called_p2, true);
    EXPECT_EQ(called_p1_after, true);
}


TEST(FutureThenTest, ValueParamReturnFutureWithValue)
{
    bool called_p1 = false;
    bool called_p2 = false;
    bool called_p1_after = false;
    double expectedValue(1.0);
    std::shared_ptr<native::Loop> currLoop = native::Loop::Create(true);
    std::thread::id mainThreadId = std::this_thread::get_id();
    {
        native::Promise<double> promise(currLoop);
        promise.getFuture().then([&called_p1, &called_p2, &expectedValue, &mainThreadId](double iValue) -> native::Future<int>{
            auto future = native::async([&called_p1, &called_p2, &mainThreadId]() -> int{
                EXPECT_EQ(called_p1, true);
                called_p2 = true;
                std::thread::id currThreadId = std::this_thread::get_id();
                EXPECT_EQ(mainThreadId, currThreadId);
                return 1;
            });
            called_p1 = true;
            EXPECT_EQ(called_p2, false);
            EXPECT_EQ(iValue, expectedValue);
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
            return future;
        }).then([&called_p1, &called_p2, &called_p1_after, &mainThreadId](int iValue){
            called_p1_after = true;
            EXPECT_EQ(iValue, 1);
            EXPECT_EQ(called_p1, true);
            EXPECT_EQ(called_p2, true);
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
        });

        EXPECT_EQ(called_p1, false);
        EXPECT_EQ(called_p2, false);
        EXPECT_EQ(called_p1_after, false);

        promise.resolve(expectedValue);

        EXPECT_EQ(called_p1, false);
        EXPECT_EQ(called_p2, false);
        EXPECT_EQ(called_p1_after, false);
    }

    EXPECT_EQ(called_p1, false);
    EXPECT_EQ(called_p2, false);
    EXPECT_EQ(called_p1_after, false);

    currLoop->run();

    EXPECT_EQ(called_p1, true);
    EXPECT_EQ(called_p2, true);
    EXPECT_EQ(called_p1_after, true);
}

