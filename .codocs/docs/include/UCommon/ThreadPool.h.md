# ThreadPool.h

## 职责

简单线程池，支持任务入队和全局单例注册。

## 关键抽象

### `FThreadPool`
- 构造时指定线程数（默认 `hardware_concurrency()`），pimpl 隐藏实现
- `Enqueue(function<void()>)` — 基础版本
- `Enqueue(F&&, Args&&...)` — 模板版本，返回 `std::future<ReturnType>`
- `GetNumThreads()` — 查询线程数
- 不可拷贝

### `FThreadPoolRegistry`
- 全局单例（`GetInstance()`），注册/注销一个 `FThreadPool*`
- `GetThreadPool()` 获取当前注册的线程池
