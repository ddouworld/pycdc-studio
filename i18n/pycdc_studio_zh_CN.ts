<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN">
<context>
    <name>DecompilerService</name>
    <message><source>[runner] pycdas = %1</source><translation>[runner] pycdas = %1</translation></message>
    <message><source>[runner] pycdc = %1</source><translation>[runner] pycdc = %1</translation></message>
    <message><source>[session] loading %1 bytecode file(s)</source><translation>[session] 正在加载 %1 个字节码文件</translation></message>
    <message><source>Running decompilers (%1/%2): %3</source><translation>正在运行反编译器（%1/%2）：%3</translation></message>
    <message><source>[session] opened %1</source><translation>[session] 已打开 %1</translation></message>
    <message><source>[pycdas] %1 completed successfully</source><translation>[pycdas] %1 执行成功</translation></message>
    <message><source>[pycdas] %1</source><translation>[pycdas] %1</translation></message>
    <message><source>[pycdc] %1 completed successfully</source><translation>[pycdc] %1 执行成功</translation></message>
    <message><source>[pycdc] %1</source><translation>[pycdc] %1</translation></message>
    <message><source>Native decompilation finished.</source><translation>原生反编译已完成。</translation></message>
    <message><source>Native decompilation failed. AI fallback can be added next.</source><translation>原生反编译失败，可以继续使用 AI 兜底重建。</translation></message>
    <message><source>Loaded %1 bytecode files into the workspace.</source><translation>已将 %1 个字节码文件加载到工作区。</translation></message>
</context>
<context>
    <name>FallbackService</name>
    <message><source>[ai] node not found: %1</source><translation>[ai] 未找到节点：%1</translation></message>
    <message><source>[ai] prepared fallback prompt for %1</source><translation>[ai] 已为 %1 准备兜底提示词</translation></message>
    <message><source>AI provider is not configured. Falling back to placeholder source.</source><translation>AI 服务尚未配置，当前回退到占位源码。</translation></message>
    <message><source>[ai] provider not configured; using placeholder output</source><translation>[ai] 服务未配置；使用占位输出</translation></message>
    <message><source>Requesting AI fallback for %1...</source><translation>正在为 %1 请求 AI 兜底重建...</translation></message>
    <message><source>AI fallback failed for %1</source><translation>%1 的 AI 兜底重建失败</translation></message>
    <message><source>[ai] request failed: %1</source><translation>[ai] 请求失败：%1</translation></message>
    <message><source>AI fallback completed for %1</source><translation>%1 的 AI 兜底重建已完成</translation></message>
    <message><source>[ai] reconstructed %1</source><translation>[ai] 已重建 %1</translation></message>
</context>
<context>
    <name>MainWindow</name>
    <message><source>pycdc-studio</source><translation>pycdc-studio</translation></message>
    <message><source>A desktop workspace for Python bytecode analysis, native decompilation, and AI-assisted fallback reconstruction.</source><translation>一个用于 Python 字节码分析、原生反编译与 AI 兜底重建的桌面工作区。</translation></message>
    <message><source>Retry with AI</source><translation>使用 AI 重试</translation></message>
    <message><source>Save Merged</source><translation>保存合并结果</translation></message>
    <message><source>Send the currently selected code object to AI fallback reconstruction</source><translation>将当前选中的 code object 发送给 AI 进行兜底重建</translation></message>
    <message><source>Save the merged result for the currently selected file</source><translation>保存当前所选文件的合并结果</translation></message>
    <message><source>Works on the selected function, method, class body, or module node.</source><translation>可作用于当前选中的函数、方法、类体或模块节点。</translation></message>
    <message><source>Drop bytecode or a folder</source><translation>拖入字节码文件或文件夹</translation></message>
    <message><source>Drag .pyc/.pyo files or a folder. Directories are scanned recursively for supported bytecode.</source><translation>可拖入 .pyc/.pyo 文件或文件夹。目录会递归扫描支持的字节码文件。</translation></message>
    <message><source>Compare outputs</source><translation>对比输出结果</translation></message>
    <message><source>Switch between merged, native, and AI-reconstructed source with shared metadata.</source><translation>可在合并结果、原生结果和 AI 重建结果之间切换，并共享同一份元数据。</translation></message>
    <message><source>Inspect prompt context</source><translation>查看提示词上下文</translation></message>
    <message><source>Review metadata, disassembly, and the exact AI prompt used for reconstruction.</source><translation>查看元数据、反汇编结果，以及实际发送给 AI 的重建提示词。</translation></message>
    <message><source>Name</source><translation>名称</translation></message>
    <message><source>Type</source><translation>类型</translation></message>
    <message><source>Status</source><translation>状态</translation></message>
    <message><source>Merged</source><translation>合并结果</translation></message>
    <message><source>Native</source><translation>原生结果</translation></message>
    <message><source>AI</source><translation>AI</translation></message>
    <message><source>Disassembly</source><translation>反汇编</translation></message>
    <message><source>Metadata</source><translation>元数据</translation></message>
    <message><source>Prompt</source><translation>提示词</translation></message>
    <message><source>Log</source><translation>日志</translation></message>
    <message><source>Ready</source><translation>就绪</translation></message>
    <message><source>File</source><translation>文件</translation></message>
    <message><source>Open .pyc...</source><translation>打开 .pyc...</translation></message>
    <message><source>Save Merged Result...</source><translation>保存合并结果...</translation></message>
    <message><source>Settings</source><translation>设置</translation></message>
    <message><source>Exit</source><translation>退出</translation></message>
    <message><source>No .pyc or .pyo files were found in the dropped selection.</source><translation>拖入内容中没有找到 .pyc 或 .pyo 文件。</translation></message>
    <message><source>[drop] no supported bytecode files found</source><translation>[drop] 未找到支持的字节码文件</translation></message>
    <message><source>Opened dropped bytecode file: %1</source><translation>已打开拖入的字节码文件：%1</translation></message>
    <message><source>[drop] discovered %1 supported bytecode files</source><translation>[drop] 发现 %1 个支持的字节码文件</translation></message>
    <message><source>[drop] %1</source><translation>[drop] %1</translation></message>
    <message><source>[drop] ... and %1 more</source><translation>[drop] ... 以及另外 %1 个</translation></message>
    <message><source>Unable to open the dropped bytecode selection.</source><translation>无法打开拖入的字节码内容。</translation></message>
    <message><source>Open Python Bytecode</source><translation>打开 Python 字节码</translation></message>
    <message><source>There is no merged result to save.</source><translation>当前没有可保存的合并结果。</translation></message>
    <message><source>Save Merged Result</source><translation>保存合并结果</translation></message>
    <message><source>Python Source (*.py);;Text Files (*.txt);;All Files (*)</source><translation>Python 源码 (*.py);;文本文件 (*.txt);;所有文件 (*)</translation></message>
    <message><source>Unable to save the merged result to %1.</source><translation>无法将合并结果保存到 %1。</translation></message>
    <message><source>Saved merged result to %1</source><translation>已将合并结果保存到 %1</translation></message>
    <message><source>[save] merged result written to %1</source><translation>[save] 已将合并结果写入 %1</translation></message>
    <message><source>Python Bytecode (*.pyc *.pyo);;All Files (*)</source><translation>Python 字节码 (*.pyc *.pyo);;所有文件 (*)</translation></message>
    <message><source>Settings saved.</source><translation>设置已保存。</translation></message>
    <message><source>[settings] application settings updated</source><translation>[settings] 应用设置已更新</translation></message>
    <message><source>No file</source><translation>无文件</translation></message>
    <message><source>Qualified Name: %1
</source><translation>限定名称：%1
</translation></message>
    <message><source>Type: %1
</source><translation>类型：%1
</translation></message>
    <message><source>Status: %1
</source><translation>状态：%1
</translation></message>
    <message><source>Source File: %1
</source><translation>源文件：%1
</translation></message>
    <message><source>First Line: %1
</source><translation>首行：%1
</translation></message>
    <message><source>co_names</source><translation>co_names</translation></message>
    <message><source>Locals+Names</source><translation>Locals+Names</translation></message>
    <message><source>Free Vars</source><translation>自由变量</translation></message>
    <message><source>Cell Vars</source><translation>闭包变量</translation></message>
    <message><source>co_consts</source><translation>co_consts</translation></message>
    <message><source>
Native Error:
%1</source><translation>
原生错误：
%1</translation></message>
    <message><source>Ctrl+R</source><translation>Ctrl+R</translation></message>
    <message><source>Language Restart Required</source><translation>需要重启以应用语言</translation></message>
    <message><source>The language setting was saved, but the application could not be restarted automatically. Please restart it manually.</source><translation>语言设置已保存，但应用无法自动重启。请手动重启应用。</translation></message>
    <!-- 新增：主界面 provider/model 选择行 -->
    <message><source>Provider:</source><translation>服务商：</translation></message>
    <message><source>Model:</source><translation>模型：</translation></message>
    <message><source>Active AI provider used for reconstruction</source><translation>用于 AI 重建的当前服务商</translation></message>
    <message><source>Model to use for AI reconstruction</source><translation>用于 AI 重建的模型</translation></message>
    <message><source>(unnamed)</source><translation>（未命名）</translation></message>
</context>
<context>
    <name>ModelPickerDialog</name>
    <message><source>Select Models</source><translation>选择模型</translation></message>
    <message><source>Model Manager</source><translation>模型管理</translation></message>
    <message><source>Add models manually or fetch from the provider. Check items to enable. Click a row to select it as the active model.</source><translation>可手动添加模型或从服务商拉取。勾选启用，点击行选中为当前模型。</translation></message>
    <message><source>Enter model name, e.g. gpt-4o-mini</source><translation>输入模型名，例如 gpt-4o-mini</translation></message>
    <message><source>Add</source><translation>添加</translation></message>
    <message><source>Fetch Models</source><translation>拉取模型列表</translation></message>
    <message><source>Fetch available models from the provider&apos;s /models endpoint</source><translation>从服务商的 /models 接口拉取可用模型列表</translation></message>
    <message><source>Test Checked</source><translation>测试勾选项</translation></message>
    <message><source>Test all checked models and show status and latency per model</source><translation>对所有勾选的模型逐一发起测试，在每行显示状态和响应时间</translation></message>
    <message><source>Remove</source><translation>移除</translation></message>
    <message><source>No test run yet.</source><translation>尚未进行测试。</translation></message>
    <message><source>Use Selected</source><translation>使用选中模型</translation></message>
    <message><source>Cancel</source><translation>取消</translation></message>
    <message><source>Base URL or API Key is missing; cannot fetch models.</source><translation>Base URL 或 API Key 未填写，无法拉取模型列表。</translation></message>
    <message><source>Fetching models...</source><translation>正在拉取模型列表...</translation></message>
    <message><source>Request was canceled.</source><translation>请求已取消。</translation></message>
    <message><source>Fetch timed out.</source><translation>拉取超时。</translation></message>
    <message><source>Failed to parse response from /models.</source><translation>解析 /models 响应失败。</translation></message>
    <message><source>Unknown error</source><translation>未知错误</translation></message>
    <message><source>API error: %1</source><translation>API 错误：%1</translation></message>
    <message><source>No models returned by the provider.</source><translation>服务商未返回任何模型。</translation></message>
    <message><source>Fetched %1 model(s); %2 new added.</source><translation>已拉取 %1 个模型，新增 %2 个。</translation></message>
    <message><source>No checked models to test.</source><translation>没有勾选的模型可供测试。</translation></message>
    <message><source>Base URL or API Key is missing; cannot test.</source><translation>Base URL 或 API Key 未填写，无法测试。</translation></message>
    <message><source>Testing %1 checked model(s)...</source><translation>正在测试 %1 个勾选的模型...</translation></message>
    <message><source>Testing...</source><translation>测试中...</translation></message>
    <message><source>Response received.</source><translation>已收到响应。</translation></message>
    <message><source>API error</source><translation>API 错误</translation></message>
    <message><source>Unexpected response format.</source><translation>响应格式异常。</translation></message>
    <message><source>HTTP %1</source><translation>HTTP %1</translation></message>
    <message><source>Invalid JSON response.</source><translation>无效的 JSON 响应。</translation></message>
    <message><source>Test canceled.</source><translation>测试已取消。</translation></message>
    <message><source>Timed out</source><translation>超时</translation></message>
    <message><source>All tests completed.</source><translation>全部测试完成。</translation></message>
    <message><source>OK  %1 ms</source><translation>成功  %1 ms</translation></message>
    <message><source>FAIL  %1 ms</source><translation>失败  %1 ms</translation></message>
</context>
<context>
    <name>OpenAiCompatibleClient</name>
    <message><source>AI provider is not configured. Set PYCDC_STUDIO_AI_BASE_URL, PYCDC_STUDIO_AI_API_KEY, and PYCDC_STUDIO_AI_MODEL.</source><translation>AI 服务尚未配置。请设置 PYCDC_STUDIO_AI_BASE_URL、PYCDC_STUDIO_AI_API_KEY 和 PYCDC_STUDIO_AI_MODEL。</translation></message>
    <message><source>AI request was canceled unexpectedly.</source><translation>AI 请求被意外取消。</translation></message>
    <message><source>AI request timed out.</source><translation>AI 请求超时。</translation></message>
    <message><source>HTTP %1: %2</source><translation>HTTP %1：%2</translation></message>
    <message><source>AI response did not contain usable source text.</source><translation>AI 响应中没有可用的源码文本。</translation></message>
    <message><source>Failed to parse AI response JSON.</source><translation>解析 AI 响应 JSON 失败。</translation></message>
    <message><source>Unknown AI error.</source><translation>未知 AI 错误。</translation></message>
    <message><source>AI response format is not supported yet.</source><translation>当前尚不支持这种 AI 响应格式。</translation></message>
</context>
<context>
    <name>PycdasProcessRunner</name>
    <message><source>invalid-input</source><translation>invalid-input</translation></message>
    <message><source>Input file path is empty.</source><translation>输入文件路径为空。</translation></message>
    <message><source>process-start-failed</source><translation>process-start-failed</translation></message>
    <message><source>Failed to start pycdas from '%1'. Make sure the executable exists and is accessible.</source><translation>无法从"%1"启动 pycdas。请确认可执行文件存在且可访问。</translation></message>
    <message><source>disassemble-failed</source><translation>disassemble-failed</translation></message>
    <message><source>pycdas exited with code %1.</source><translation>pycdas 退出码为 %1。</translation></message>
</context>
<context>
    <name>PycdcProcessRunner</name>
    <message><source>invalid-input</source><translation>invalid-input</translation></message>
    <message><source>Input file path is empty.</source><translation>输入文件路径为空。</translation></message>
    <message><source>process-start-failed</source><translation>process-start-failed</translation></message>
    <message><source>Failed to start pycdc from '%1'. Make sure the executable exists and is accessible.</source><translation>无法从"%1"启动 pycdc。请确认可执行文件存在且可访问。</translation></message>
    <message><source>decompile-failed</source><translation>decompile-failed</translation></message>
    <message><source>pycdc exited with code %1.</source><translation>pycdc 退出码为 %1。</translation></message>
</context>
<context>
    <name>SettingsDialog</name>
    <message><source>Settings</source><translation>设置</translation></message>
    <message><source>Application Settings</source><translation>应用设置</translation></message>
    <message><source>Manage AI providers and models. The active provider is used for AI reconstruction. Language changes apply after restart.</source><translation>管理 AI 服务商与模型。当前激活的服务商用于 AI 重建。语言切换需重启后生效。</translation></message>
    <message><source>Providers</source><translation>服务商</translation></message>
    <message><source>Add new provider</source><translation>添加服务商</translation></message>
    <message><source>Remove selected provider</source><translation>移除所选服务商</translation></message>
    <message><source>Move up</source><translation>上移</translation></message>
    <message><source>Move down</source><translation>下移</translation></message>
    <message><source>Add</source><translation>添加</translation></message>
    <message><source>Remove</source><translation>移除</translation></message>
    <message><source>Provider Configuration</source><translation>服务商配置</translation></message>
    <message><source>Name</source><translation>名称</translation></message>
    <message><source>e.g. OpenAI, DeepSeek, Local Ollama</source><translation>例如 OpenAI、DeepSeek、本地 Ollama</translation></message>
    <message><source>Base URL</source><translation>Base URL</translation></message>
    <message><source>https://api.example.com/v1</source><translation>https://api.example.com/v1</translation></message>
    <message><source>API Key</source><translation>API Key</translation></message>
    <message><source>sk-...</source><translation>sk-...</translation></message>
    <message><source>Model</source><translation>模型</translation></message>
    <message><source>gpt-4o-mini / qwen-plus / ...</source><translation>gpt-4o-mini / qwen-plus / ...</translation></message>
    <message><source>Select Models...</source><translation>选择模型...</translation></message>
    <message><source>Open model manager to add, fetch, test and select models</source><translation>打开模型管理器，可添加、拉取、测试并选择模型</translation></message>
    <message><source>System Prompt</source><translation>System Prompt</translation></message>
    <message><source>Optional custom system prompt</source><translation>可选的自定义 system prompt</translation></message>
    <message><source>Language:</source><translation>语言：</translation></message>
    <message><source>English</source><translation>English</translation></message>
    <message><source>简体中文</source><translation>简体中文</translation></message>
    <message><source>Save</source><translation>保存</translation></message>
    <message><source>Cancel</source><translation>取消</translation></message>
    <message><source>Default</source><translation>默认</translation></message>
    <message><source>New Provider</source><translation>新服务商</translation></message>
    <message><source>(unnamed)</source><translation>（未命名）</translation></message>
</context>
</TS>
