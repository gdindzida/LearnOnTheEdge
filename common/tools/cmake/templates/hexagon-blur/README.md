# Hexagon blur template

## Navigation

- [Home](../../../../../README.md)
- [Parent: CMake templates](../README.md)

This template generates a deterministic 100×100, 8-bit grayscale image and applies a 3×3 box blur. The same kernel can run locally on the CPU or remotely on the QCS6490 Hexagon CDSP through FastRPC.

The filter clamps coordinates at the image edges and truncates the nine-pixel average to an integer. The program writes binary PGM files named `input.pgm` and `blurred.pgm`.

## Command-line interface

```text
image_blur_app --backend cpu|dsp [--seed UINT32] [--output-dir PATH]
```

`--backend cpu` runs the kernel in the ARM process. `--backend dsp` opens an unsigned CDSP protection domain, sends the image through the generated FastRPC stub, and compares every returned pixel with the CPU result. `--seed` defaults to `42`; `--output-dir` defaults to `output`.

## Build the CPU version

```bash
cmake -S . -B build/native -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/native
ctest --test-dir build/native --output-on-failure
build/native/image_blur_app --backend cpu --seed 42 --output-dir results/native
```

The native CPU-only binary deliberately rejects `--backend dsp` because it is not linked with FastRPC.

For GCC and Clang-compatible compilers, Release builds add `-O2` to C and C++
compilation. Other build types retain their usual compiler options.

## Build the ARM host and Hexagon DSP objects

Set up the SDK first:

```bash
cd /local/mnt/workspace/Qualcomm/Hexagon_SDK/6.6.0.0
source setup_sdk_env.source
```

Then configure and build from this directory:

```bash
cd /home/mdzida/workspace/projects/LearnOnTheEdge/common/tools/cmake/templates/hexagon-blur
export UBUNTUARM_TOOLS_DIR="$PWD/../../../aarch64/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu"
export UBUNTUARM_LIB_DIR="$UBUNTUARM_TOOLS_DIR/lib/gcc/aarch64-none-linux-gnu/13.3.1"
build_cmake ubuntuARM BUILD=ReleaseG HLOS_ARCH=64 BUILD_OUTPUT_DIR=build/arm -j4
build_cmake hexagon BUILD=ReleaseG DSP_ARCH=v68 NO_QURT_INC=1 BUILD_OUTPUT_DIR=build/dsp -j4
```

The important artifacts are:

```text
build/arm/ship/image_blur_app
build/arm/ship/libimage_blur.so
build/dsp/ship/libimage_blur_skel.so
```

The DSP library is compiled from `dsp/image_blur_imp.c` and the shared `common/blur_kernel.c` implementation. QAIC generates the host stub and DSP skeleton from `inc/image_blur.idl`.

## Deploy and run on a Radxa Dragon Q6A

The checked-in manifest copies the ARM executable, host stub, and DSP skeleton to `~/test/hexagon-blur`, then sets the loader paths before launching the app:

```bash
manifest=common/tools/cmake/templates/hexagon-blur/radxa_deployment_manifest.json
common/tools/remote-controller/remote-controller upload "$manifest"
common/tools/remote-controller/remote-controller run "$manifest"
```

The default run is equivalent to:

```text
image_blur_app --backend dsp --seed 42 --output-dir output
```

A successful DSP run reports that all 10,000 pixels match the CPU reference. To change the input or backend, edit the manifest command arguments (for example, use `--backend cpu` or `--seed 1234`) and run it again. Download generated PGM files with:

```bash
common/tools/remote-controller/remote-controller download "$manifest"
```

The manifest cleanup command removes only `~/test/hexagon-blur`.

## Simulator scope

This template currently provides a native CPU build and a physical-device DSP build. It does not define a `hexagonsim` executable: the C++ front end uses FastRPC, while Hexagon Simulator needs a separate simulator entry point and invocation model. The DSP blur calculation is still covered by native golden tests and by the device-side comparison of every DSP pixel against the CPU result.
