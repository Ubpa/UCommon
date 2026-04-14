---
codocs:
  schema: 1
  source_type: file
  source_path: src/Runtime/ThreadPool.cpp
  source_hash: sha256:1efa3f9da1df448c2ff69b86d8d9840bb154ac6e0845023f14d77f35c9b6b7dc
  explicit_deps: []
  dep_hash: sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  hash_mode: text-lf-sha256
  verified_at: '2026-04-14T17:16:35.376791+08:00'
---
# ThreadPool.cpp

## 职责

简单的 C++11 风格线程池实现。

## 关键抽象

- `FThreadPool` — 固定线程数的任务队列线程池，支持 `Enqueue` 提交任务
- `FThreadPoolRegistry` — 全局单例注册表，允许注册/获取一个默认线程池

## 实现要点

- 经典 mutex + condition_variable 模式：工作线程阻塞等待任务，notify_one 唤醒
- 析构时设置 bStop 并 notify_all，等待所有线程 join
- Enqueue 在线程池已停止时返回 false（不抛异常）
- Impl 使用 placement new + `UBPA_UCOMMON_MALLOC/FREE`
