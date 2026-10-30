---
name: cmake-template-project
description: Create a fresh CMake project from a LearnOnTheEdge template. Use when selecting, copying, renaming, building, or cross-compiling the basic, library, gtest, benchmark, or hexagon-blur templates.
---

# CMake Template Project

Use this skill for a new project based on a template in
`common/tools/cmake/templates/`. Do not use it to retrofit an existing project
unless the user explicitly asks to replace that project with a template.

## Discover the right template

Read `common/tools/cmake/templates/README.md` and the selected template README
before changing files. Select one template:

- `basic` for a minimal C++ executable.
- `library` for static and shared libraries with one small executable per variant.
- `gtest` for a C++ library with GoogleTest and CTest.
- `benchmark` for a Google Benchmark executable.
- `hexagon-blur` for the CPU, UbuntuARM, and Hexagon DSP FastRPC example.

Before creating files, obtain the template, destination, new project name, and
build target. Ask for any missing value that would change the destination or
project identity. For a nonempty destination, ask before overwriting or merging.

## Create the project

Copy the selected template into the destination without copying any `build/`
directories, compile databases, generated outputs, or other local artifacts.
Prefer a copy operation that excludes `build/` recursively. Keep the template
source unchanged.

Rename only identifiers and files that the user has asked to rename. Update the
selected `CMakeLists.txt`, affected source and header names, documentation, and
deployment paths together. Preserve target relationships and public include
paths unless the requested project design changes them.

For `basic`, rename the `project()` name and executable target. For `library`,
rename both library targets and both matching applications consistently. For
`gtest`, rename the library and test target together. For `benchmark`, rename
the executable target and benchmark names when requested. For `hexagon-blur`,
read its README first and change its IDL names, generated-artifact references,
and deployment manifest only as one coordinated change.

## Configure and verify

Use Ninja when it is installed; otherwise select `Unix Makefiles`. Use separate
build directories for native x86-64 and AArch64 builds. The general-purpose
templates require CMake 3.20 and C++17. Use the selected template README for
its executable or test command.

For AArch64 builds, read `common/tools/aarch64/README.md`, set `REPO_ROOT` to
an absolute path, run its setup when needed, and configure with
`$REPO_ROOT/common/tools/aarch64/toolchain.cmake`. Use `$HOME` or an absolute
path in shell commands; never use `"$~/..."`. Build tests for cross targets,
but run target executables and tests on the Q6A rather than the build host.

## Configure clangd

The templates set `CMAKE_EXPORT_COMPILE_COMMANDS` on, so a successful configure
creates `compile_commands.json` in the selected build directory. After that
file exists, update the nearest project `.clangd` with one YAML fragment for
the new project. Match the project source path relative to that `.clangd` file
and set `CompileFlags.CompilationDatabase` to the matching build directory.

Use the database for the build the user intends to edit: `build/linux-x64` for
native general-purpose projects, `build/linux-aarch64` for AArch64,
`build/native` for Hexagon CPU work, `build/arm` for the UbuntuARM host, and
`build/dsp` for DSP implementation sources. Preserve other `.clangd` fragments;
update a matching fragment when present or append a separate fragment. Do not
point clangd at a build directory until its `compile_commands.json` exists.

For a cross build, derive any required sysroot include paths from the configured
toolchain rather than hardcoding a user home directory. When `clangd` is
available, run `clangd --check` for a representative source with the selected
compile-commands directory and fix only configuration errors reported there.

`gtest` and `benchmark` fetch pinned dependencies on their first configure.
Tell the user before that configure requires Git and network access. Use
`BUILD_TESTING=OFF` for the gtest template only when the user wants the library
without tests.

For `hexagon-blur`, validate the native CPU build with its CTest command. Build
UbuntuARM and Hexagon DSP artifacts only after the user has set up the Qualcomm
Hexagon SDK environment described in that template README. Do not substitute the
general AArch64 toolchain for the SDK FastRPC workflow.

## Completion

Report the source template, destination, identifiers renamed, build generator,
and validation performed. State any skipped validation and why, including a
missing SDK, target hardware, or dependency network access.
