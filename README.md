# pycdc-studio

[中文说明](./README_CN.md)

A Qt Widgets desktop UI for exploring Python bytecode with `pycdc` / `pycdas`, inspecting native decompilation results, importing Pyarmor oneshot output, and retrying unsupported code objects with AI fallback.

The app supports both Windows and Linux desktop environments.

## Screenshots

### Main workspace

![Main workspace](./docs/images/main-workspace-en.png)

### AI fallback

![AI fallback reconstruction](./docs/images/ai-fallback-en.png)

## What it does

- Open `.pyc` / `.pyo` files directly
- Drag a folder into the window and recursively discover supported bytecode files
- Import a Pyarmor-obfuscated project through the bundled oneshot workflow
- Show all discovered files together in the left tree
- Inspect code object trees for modules, classes, functions, lambdas, and comprehensions
- Compare:
  - `Merged`
  - `Native`
  - `AI`
- View per-node metadata and disassembly
- Preview the exact prompt sent to the AI fallback model
- Retry the currently selected node with AI using a prominent action button or `Ctrl+R`

## Current UI structure

- Left: code object tree
- Center: merged / native / AI source views
- Right: disassembly / metadata / prompt / log

## AI fallback

The app does not send the whole `.pyc` by default.

It builds a prompt from the currently selected code object, including:

- qualified name
- object type
- names / varnames / consts previews
- native error
- disassembly

That keeps fallback reconstruction focused on the selected node instead of the whole file.

## pycdc integration

The app currently prefers local executables next to the application binary and will look for:

- `pycdc.exe`
- `pycdc`
- `pycdas.exe`
- `pycdas`

in the same directory as the `pycdc-studio` application binary.

You can override them with environment variables:

- `PYCDC_STUDIO_PYCDC`
- `PYCDC_STUDIO_PYCDAS`

## Pyarmor import

The app can import Pyarmor-protected projects by delegating to
[`Lil-House/Pyarmor-Static-Unpack-1shot`](https://github.com/Lil-House/Pyarmor-Static-Unpack-1shot).

Current integration flow:

1. Run `oneshot/shot.py` on a selected Pyarmor project directory
2. Let `pyarmor-1shot` generate `.1shot.das` and `.1shot.cdc.py`
3. Load those generated files back into the existing workspace UI

Release packages are expected to bundle the Pyarmor oneshot toolchain and the
required Python package dependencies automatically.

For local development, the current code first looks for bundled files next to
the application package, and also supports a local clone at:

- `external/Pyarmor-Static-Unpack-1shot/oneshot/shot.py`

and expects `pyarmor-1shot` / `pyarmor-1shot.exe` to be available in the same
directory as `shot.py`.

You also need a Python environment that can run `shot.py`, including
`pycryptodome`.

If your layout is different, you can still override it with environment variables:

- `PYCDC_STUDIO_PYARMOR_PYTHON`
- `PYCDC_STUDIO_PYARMOR_SHOT`
- `PYCDC_STUDIO_PYARMOR_PYTHONPATH`
- `PYCDC_STUDIO_PYARMOR_OUTPUT_ROOT`

### Prepare the upstream tool for local development

Example:

```bash
git clone https://github.com/Lil-House/Pyarmor-Static-Unpack-1shot external/Pyarmor-Static-Unpack-1shot
cmake -S external/Pyarmor-Static-Unpack-1shot/pycdc -B external/Pyarmor-Static-Unpack-1shot/build
cmake --build external/Pyarmor-Static-Unpack-1shot/build --config Release
cmake --install external/Pyarmor-Static-Unpack-1shot/build --config Release
python -m pip install pycryptodome
```

### Use it in the UI

1. Launch `pycdc-studio`
2. Open `File -> Import Pyarmor Project...`
3. Choose the root directory that contains the obfuscated scripts and `pyarmor_runtime`
4. Wait for the generated `.1shot.*` files to be imported into the workspace

Notes:

- The importer uses a temporary output directory unless `PYCDC_STUDIO_PYARMOR_OUTPUT_ROOT` is set.
- Disassembly is generally more reliable than decompiled source.
- Archives such as PyInstaller bundles still need to be unpacked before using this flow.
- Packaged builds are intended to work without manually configuring Pyarmor environment variables.

## AI configuration

Before using AI fallback, open `Settings` and configure at least:

- `Base URL`
- `API Key`
- `Model`

The current client expects an **OpenAI-compatible API** endpoint.

The settings dialog stores provider configuration with `QSettings` and falls back to environment variables when a field is empty.

Supported environment variables:

- `PYCDC_STUDIO_AI_BASE_URL`
- `PYCDC_STUDIO_AI_API_KEY`
- `PYCDC_STUDIO_AI_MODEL`
- `PYCDC_STUDIO_AI_SYSTEM_PROMPT`

## Build

Requirements:

- Qt 6 Widgets
- Qt Network
- CMake
- C++17 compiler

Example:

```bash
cmake -S . -B build
cmake --build build
```

## Usage

1. Launch `pycdc-studio`
2. Open a `.pyc` / `.pyo` file, or drag a folder into the window
3. Or open `File -> Import Pyarmor Project...` for a Pyarmor-protected project
4. Open `Settings` from the top-level menu and configure your AI model if you want to use AI fallback
5. Select a file or code object in the tree
6. Inspect native output, disassembly, metadata, and prompt context
7. Use `Retry with AI` on the selected node if native decompilation is incomplete or wrong

## Test samples

Longer test cases are included under `test/`:

- `workflow_orchestrator.py`
- `async_batch_runner.py`
- `plugin_config_resolver.py`
- plus several smaller samples

The repository tracks the **source** samples only.

To generate local `.pyc` files for drag-and-drop testing:

```bash
python test/compile_test_samples.py
```

That script writes the compiled bytecode into `test/__pycache__/`.

## Notes

This project is still experimental.

The current `Merged` view is an honest mixed document:
- native source when available
- AI fallback patches appended per code object

It is not yet a full inline source merger.

## License

This project integrates with `pycdc` / `pycdas`, whose upstream project is licensed under **GPL-3.0**.

- `pycdc`: GPL-3.0
- release packages that bundle `pycdc` / `pycdas` should include the GPL-3.0 notice
- release packages also record the bundled upstream `pycdc` repository and commit in `THIRD_PARTY_NOTICES.txt`

See the upstream project for full license details:
- [zrax/pycdc](https://github.com/zrax/pycdc)
