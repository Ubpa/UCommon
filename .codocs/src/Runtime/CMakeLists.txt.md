# CMakeLists.txt

构建 UCommon Runtime 静态/动态库（由 `Ubpa_UCommon_UsingSharedLibrary` 控制）。

- 静态库（默认）：无额外 define；动态库：`UBPA_UCOMMON_USING_SHARED_LIBRARY`（公共）+ `UBPA_UCOMMON_BUILDING_SHARED_LIBRARY`（私有）
- C++17；头文件来源 `include/UCommon`（通过 SOURCE 参数，UCMake 自动收集）
- MSVC 屏蔽警告：4251（dll-interface）、4334（>>不会截断）、4267（size_t 截断）等
