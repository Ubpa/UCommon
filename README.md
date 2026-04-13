# UCommon

> **U**bpa **Common**

## Git Hooks

本仓库使用 **more-hooks** 和 **codocs** 两套 hook 工具，通过 more-hooks dispatcher 统一调度。

### more-hooks

more-hooks 是 git hook 的多工具调度框架，将各工具的 hook 脚本注册到 `.git/hooks.d/<hook-name>/` 下，由 `.git/hooks/<hook-name>` dispatcher 按优先级顺序调用。

当前注册的 hook（`50-codocs`）：

```
.git/hooks/
├── pre-commit       ← dispatcher
└── commit-msg       ← dispatcher

.git/hooks.d/
├── pre-commit/
│   └── 50-codocs    ← codocs pre-commit hook
└── commit-msg/
    └── 50-codocs    ← codocs commit-msg hook
```

### codocs

codocs 是代码语义文档系统，在 `.codocs/` 下维护与代码树严格镜像的 Markdown 文档。

**管控范围**（见 `.codocs/config.json`）：

```json
{
  "roots": ["include/UCommon", "src/Runtime"],
  "exclude": ["*.backup", "*.inl"],
  "exclude_paths": ["include/UCommon/_deps", "src/Runtime/CMakeLists.txt"]
}
```

**Hook 行为**：

| Phase | 触发条件 | 行为 |
|-------|----------|------|
| 1 (pre-commit) | lint：存在 ORPHAN/MISSING 文档 | 硬拦截，必须修复 |
| 2 (pre-commit + commit-msg) | 受管控代码有改动但无 `.codocs/` 改动 | 警告，要求在 commit message 中声明 |
| 3 (pre-commit + commit-msg) | `.codocs/` 子文档已更新但父目录 MD 未同步 | 警告，要求声明 |

**跳过声明**（在 commit message 末尾添加）：

```
## codocs-skip
- .codocs/src/Runtime/Foo.cpp.md: 仅修改内部实现，接口描述未变
- .codocs/include/UCommon.md: 未新增/删除/改名文件，索引表不受影响
```

### 重新安装 hooks

如果 `.git/hooks/` 被意外清空，重新运行 codocs init 即可（会自动安装 more-hooks dispatcher 和 codocs hook）：

```bash
python .codocs/scripts/codocs.py .
```
