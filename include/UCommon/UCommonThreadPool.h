/*/*
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

#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

#define UBPA_UCOMMON_THREAD_POOL_TO_NAMESPACE(NameSpace) \
namespace NameSpace \
{ \
	using FThreadPool = UCommon::FThreadPool; \
}

namespace UCommon
{
    class FThreadPool
    {
    public:
        FThreadPool(uint64_t NumThread);
        ~FThreadPool();

        /** Add new work item to the pool. */
        template<class F, class... Args>
        std::future<std::invoke_result_t<F, Args...>> Enqueue(F&& Function, Args&&... Arguments)
        {
            using return_type = std::invoke_result_t<F, Args...>;

            auto task = std::make_shared< std::packaged_task<return_type()> >(
                std::bind(std::forward<F>(Function), std::forward<Args>(Arguments)...)
                );

            std::future<return_type> res = task->get_future();
            {
                std::unique_lock<std::mutex> lock(QueueMutex);

                // don't allow enqueueing after stopping the pool
                if (bStop)
                {
                    // throw std::runtime_error("enqueue on stopped ThreadPool");
                    return std::future<return_type>();
                }

                Tasks.emplace([task]() { (*task)(); });
            }
            Condition.notify_one();
            return res;
        }

        size_t GetNumThreads() const noexcept { return Workers.size(); }

        FThreadPool(const FThreadPool&) = delete;
        FThreadPool& operator=(const FThreadPool&) = delete;
        
    private:
        /** need to keep track of threads so we can join them. */
        std::vector< std::thread > Workers;
        /** the task queue */
        std::queue< std::function<void()> > Tasks;

        /** synchronization */
        std::mutex QueueMutex;
        std::condition_variable Condition;
        bool bStop;
    };
}
