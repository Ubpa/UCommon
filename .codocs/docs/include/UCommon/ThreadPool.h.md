---
codocs:
  schema: 1
  source_type: file
  source_path: include/UCommon/ThreadPool.h
  source_hash: sha256:65fd81cf1f0a41688c6684557bb1e8aaab8419978f69d968b47d9711acfaae32
  explicit_deps: []
  dep_hash: sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  hash_mode: text-lf-sha256
  verified_at: '2026-04-14T17:16:35.376791+08:00'
---
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
