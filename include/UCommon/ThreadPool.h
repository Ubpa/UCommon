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

#include "Cpp17.h"

#include <functional>
#include <future>

#define UBPA_UCOMMON_THREAD_POOL_TO_NAMESPACE(NameSpace) \
namespace NameSpace \
{ \
	using FThreadPool = UCommon::FThreadPool; \
	using FThreadPoolRegistry = UCommon::FThreadPoolRegistry; \
}

namespace UCommon
{
    class UBPA_UCOMMON_API FThreadPool
    {
        struct FImpl;
        FImpl* Impl;
    public:
        FThreadPool(uint64_t NumThread = std::thread::hardware_concurrency());
        ~FThreadPool();

        bool Enqueue(std::function<void()> Function);

        /** Add new work item to the pool. */
        template<class F, class... Args>
        std::future<InvokeResult_t<F, Args...>> Enqueue(F&& Function, Args&&... Arguments)
        {
            using return_type = InvokeResult_t<F, Args...>;

            auto task = new std::packaged_task<return_type()>(
                std::bind(std::forward<F>(Function), std::forward<Args>(Arguments)...)
                );

            std::future<return_type> res = task->get_future();

            if (!Enqueue(std::function<void()>([task]() { (*task)(); delete task; })))
            {
                return std::future<return_type>();
            }

            return res;
        }

        size_t GetNumThreads() const noexcept;

        FThreadPool(const FThreadPool&) = delete;
        FThreadPool& operator=(const FThreadPool&) = delete;
    };

    class UBPA_UCOMMON_API FThreadPoolRegistry
    {
    public:
        static FThreadPoolRegistry& GetInstance();
        void Register(FThreadPool* InThreadPool) noexcept;
        void Deregister() noexcept;
        FThreadPool* GetThreadPool() const noexcept;
    private:
        static FThreadPoolRegistry ThreadPoolRegistry;
        FThreadPoolRegistry() = default;

        FThreadPool* ThreadPool = nullptr;
    };
}
