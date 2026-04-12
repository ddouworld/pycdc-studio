# pycdc-studio

[English README](./README.md)

一个基于 Qt Widgets 的桌面图形界面，用来配合 `pycdc` / `pycdas` 浏览 Python 字节码、查看原生反编译结果、导入 Pyarmor oneshot 输出，并在遇到不支持的 code object 时使用 AI 做兜底重建。

当前支持 Windows 和 Linux 桌面环境。

## 界面预览

### 主工作区

![主工作区](./docs/images/main-workspace.png)

### 设置页

![设置页](./docs/images/settings-dialog.png)

### AI 重建

![AI 重建效果](./docs/images/ai-fallback.png)

## 功能

- 直接打开 `.pyc` / `.pyo`
- 支持把文件夹拖进窗口，递归扫描其中的字节码文件
- 支持通过 oneshot 流程导入 Pyarmor 混淆项目
- 多文件会一起显示在左侧树里
- 查看模块、类、函数、lambda、推导式等 code object 树
- 对比三种结果：
  - `Merged`
  - `Native`
  - `AI`
- 查看每个节点的元数据和反汇编结果
- 预览发送给 AI fallback 的完整 prompt
- 通过醒目的 `Retry with AI` 按钮或 `Ctrl+R` 对当前节点做 AI 重建

## 当前界面结构

- 左侧：code object 树
- 中间：`Merged / Native / AI` 源码视图
- 右侧：`Disassembly / Metadata / Prompt / Log`

## AI fallback 是怎么做的

程序默认**不会把整个 `.pyc` 文件直接发给 AI**。

它会基于当前选中的 code object 组织 prompt，通常包含：

- qualified name
- object type
- names / locals / consts 预览
- native error
- disassembly

这样做的好处是：

- token 更省
- 噪声更少
- AI 更容易聚焦当前失败节点，而不是整个文件

## pycdc 集成方式

当前会优先查找和程序本体放在同一目录下的可执行文件：

- `pycdc.exe`
- `pycdc`
- `pycdas.exe`
- `pycdas`

也就是 `pycdc-studio` 程序本体所在目录。

也可以用环境变量覆盖：

- `PYCDC_STUDIO_PYCDC`
- `PYCDC_STUDIO_PYCDAS`

## Pyarmor 导入

当前可以通过
[`Lil-House/Pyarmor-Static-Unpack-1shot`](https://github.com/Lil-House/Pyarmor-Static-Unpack-1shot)
导入 Pyarmor 保护项目。

集成流程是：

1. 对你选中的 Pyarmor 项目目录运行 `oneshot/shot.py`
2. 由 `pyarmor-1shot` 生成 `.1shot.das` 和 `.1shot.cdc.py`
3. 再把这些生成文件加载回当前工作区

release 打包流程会自动构建并内置 Pyarmor oneshot 工具链，以及所需的
Python 依赖。

对于本地开发，当前代码会优先查找安装包里自带的文件，同时也支持这个本地 clone 路径：

- `external/Pyarmor-Static-Unpack-1shot/oneshot/shot.py`

并要求 `shot.py` 所在目录里存在：

- `pyarmor-1shot`
- 或 `pyarmor-1shot.exe`

另外还需要一个能运行 `shot.py` 的 Python 环境，并安装 `pycryptodome`。

如果你的目录布局不是这样，也可以用这些环境变量覆盖：

- `PYCDC_STUDIO_PYARMOR_PYTHON`
- `PYCDC_STUDIO_PYARMOR_SHOT`
- `PYCDC_STUDIO_PYARMOR_PYTHONPATH`
- `PYCDC_STUDIO_PYARMOR_OUTPUT_ROOT`

### 为本地开发准备上游工具

示例：

```bash
git clone https://github.com/Lil-House/Pyarmor-Static-Unpack-1shot external/Pyarmor-Static-Unpack-1shot
cmake -S external/Pyarmor-Static-Unpack-1shot/pycdc -B external/Pyarmor-Static-Unpack-1shot/build
cmake --build external/Pyarmor-Static-Unpack-1shot/build --config Release
cmake --install external/Pyarmor-Static-Unpack-1shot/build --config Release
python -m pip install pycryptodome
```

### 在界面里使用

1. 启动 `pycdc-studio`
2. 打开 `File -> Import Pyarmor Project...`
3. 选择包含混淆脚本和 `pyarmor_runtime` 的项目根目录
4. 等待 `.1shot.*` 结果生成并自动导入工作区

说明：

- 如果没有设置 `PYCDC_STUDIO_PYARMOR_OUTPUT_ROOT`，导入器会使用临时输出目录。
- 一般来说，反汇编结果比反编译源码更可靠。
- 如果目标是 PyInstaller 之类的归档包，仍然需要先用其他工具解包。
- 打包后的 release 目标是不需要用户手动配置 Pyarmor 相关环境变量。

## AI 配置

在使用 AI fallback 之前，需要先在 `设置` 中至少配置：

- `Base URL`
- `API Key`
- `Model`

当前接入的是 **兼容 OpenAI 的 API**。

设置页会通过 `QSettings` 保存 AI 配置。  
如果某个字段为空，则回退到环境变量。

支持的环境变量：

- `PYCDC_STUDIO_AI_BASE_URL`
- `PYCDC_STUDIO_AI_API_KEY`
- `PYCDC_STUDIO_AI_MODEL`
- `PYCDC_STUDIO_AI_SYSTEM_PROMPT`

## 构建

依赖：

- Qt 6 Widgets
- Qt Network
- CMake
- C++17 编译器

示例：

```bash
cmake -S . -B build
cmake --build build
```

## 使用方式

1. 启动 `pycdc-studio`
2. 打开一个 `.pyc` / `.pyo`，或者直接把文件夹拖进窗口
3. 或者对 Pyarmor 项目使用 `File -> Import Pyarmor Project...`
4. 如果要使用 AI fallback，请先通过菜单栏顶层的 `设置` 配置 AI 模型
5. 在左侧树中选择文件或某个 code object
6. 查看 native 结果、反汇编、元数据和 prompt
7. 当原生反编译不完整或不正确时，对当前节点点击 `Retry with AI`

## 测试样例

`test/` 目录下已经带了一些较长的测试样例：

- `workflow_orchestrator.py`
- `async_batch_runner.py`
- `plugin_config_resolver.py`
- 以及其他较小的样例

仓库里默认只跟踪这些 **源码样例**。

如果你要生成本地拖拽测试用的 `.pyc`，可以运行：

```bash
python test/compile_test_samples.py
```

生成结果会写到 `test/__pycache__/`。

## 说明

这个项目目前仍然是实验性质。

当前 `Merged` 视图是比较诚实的“混合结果”：

- 能原生反编译的部分显示 native source
- AI 重建部分会以 patch 的形式附加在后面

它目前**还不是**真正的 inline source merger。

## 许可证

这个项目会配合 `pycdc` / `pycdas` 使用，而它们的上游项目采用 **GPL-3.0** 许可证。

- `pycdc`：GPL-3.0
- 如果和 `pycdc` / `pycdas` 一起分发，应同时附带对应的 GPL-3.0 许可证说明
- 当前 release workflow 会在 `THIRD_PARTY_NOTICES.txt` 中记录打包所使用的 `pycdc` 上游仓库和 commit

上游项目地址：
- [zrax/pycdc](https://github.com/zrax/pycdc)
