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

#include <UCommon/UCommonThreadPool.h>

UCommon::FThreadPool::~FThreadPool()
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

UCommon::FThreadPool::FThreadPool(uint64_t NumThread)
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
            }
            );
    }
}
