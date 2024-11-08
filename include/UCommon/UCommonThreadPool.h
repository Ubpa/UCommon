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

#include "UCommonConfig.h"

#include <functional>
#include <future>
#include <type_traits>

#define UBPA_UCOMMON_THREAD_POOL_TO_NAMESPACE(NameSpace) \
namespace NameSpace \
{ \
	using FThreadPool = UCommon::FThreadPool; \
}

namespace UCommon
{
    class UBPA_UCOMMON_API FThreadPool
    {
        struct FImpl;
        FImpl* Impl;
    public:
        FThreadPool(uint64_t NumThread);
        ~FThreadPool();

        bool Enqueue(std::function<void()> Function);

        /** Add new work item to the pool. */
        template<class F, class... Args>
        std::future<typename std::result_of<F(Args...)>::type> Enqueue(F&& Function, Args&&... Arguments)
        {
            using return_type = std::result_of<F(Args...)>::type;

            auto task = std::make_unique< std::packaged_task<return_type()> >(
                std::bind(std::forward<F>(Function), std::forward<Args>(Arguments)...)
                );

            std::future<return_type> res = task->get_future();

            if (!Enqueue([localtask = std::move(task)]() { (*localtask)(); }))
            {
                return std::future<return_type>();
            }

            return res;
        }

        size_t GetNumThreads() const noexcept;

        FThreadPool(const FThreadPool&) = delete;
        FThreadPool& operator=(const FThreadPool&) = delete;
    };
}
