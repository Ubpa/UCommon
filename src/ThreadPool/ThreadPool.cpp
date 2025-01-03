/*
MIT License

Copyright (c) 2024 Ubpa

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <UCommon/ThreadPool.h>

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <stdexcept>

struct UCommon::FThreadPool::FImpl
{
    /** need to keep track of threads so we can join them. */
    std::vector< std::thread > Workers;
    /** the task queue */
    std::queue< std::function<void()> > Tasks;

    /** synchronization */
    std::mutex QueueMutex;
    std::condition_variable Condition;
    bool bStop;

    FImpl(uint64_t NumThread)
        : bStop(false)
    {
        for (uint64_t i = 0; i < NumThread; ++i)
        {
            Workers.emplace_back(
            [this]
            {
                for (;;)
                {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(this->QueueMutex);
                        this->Condition.wait(lock, [this] { return this->bStop || !this->Tasks.empty(); });
                        if (this->bStop && this->Tasks.empty())
                        {
                            return;
                        }
                        task = std::move(this->Tasks.front());
                        this->Tasks.pop();
                    }

                    task();
                }
            });
        }
    }

    ~FImpl()
    {
        {
            std::unique_lock<std::mutex> lock(QueueMutex);
            bStop = true;
        }
        Condition.notify_all();
        for (std::thread& worker : Workers)
        {
            worker.join();
        }
    }
};

UCommon::FThreadPool::FThreadPool(uint64_t NumThread)
    : Impl(new UBPA_UCOMMON_MALLOC(sizeof(FImpl))FImpl(NumThread))
{
}

UCommon::FThreadPool::~FThreadPool()
{
    UBPA_UCOMMON_ASSERT(Impl);
    Impl->~FImpl();
    UBPA_UCOMMON_FREE(Impl);
}

bool UCommon::FThreadPool::Enqueue(std::function<void()> Function)
{
    {
        std::unique_lock<std::mutex> lock(Impl->QueueMutex);

        // don't allow enqueueing after stopping the pool
        if (Impl->bStop)
        {
            // throw std::runtime_error("enqueue on stopped ThreadPool");
            return false;
        }

        Impl->Tasks.emplace(std::move(Function));
    }
    Impl->Condition.notify_one();
    return true;
}

size_t UCommon::FThreadPool::GetNumThreads() const noexcept { return Impl->Workers.size(); }

UCommon::FThreadPoolRegistry UCommon::FThreadPoolRegistry::ThreadPoolRegistry;

UCommon::FThreadPoolRegistry& UCommon::FThreadPoolRegistry::GetInstance() { return ThreadPoolRegistry; }

void UCommon::FThreadPoolRegistry::Register(FThreadPool* InThreadPool) noexcept
{
    ThreadPool = InThreadPool;
}

void UCommon::FThreadPoolRegistry::Deregister() noexcept
{
    ThreadPool = nullptr;
}

UCommon::FThreadPool* UCommon::FThreadPoolRegistry::GetThreadPool() const noexcept { return ThreadPool; }
