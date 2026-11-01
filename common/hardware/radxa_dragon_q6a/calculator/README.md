# Hexagon calculator on Radxa Dragon Q6A

## Navigation

- [Home](../../../../README.md)
- [Parent: Radxa Dragon Q6A](../README.md)

`radxa_deployment_manifest.json` deploys the Hexagon SDK 6.6.0.0 calculator
example over SSH to `radxa@radxa-dragon-q6a`. It uses the repository's
[remote-controller](../../../tools/remote-controller/README.md).

The Linux ARM64 executable calls the Hexagon v68 DSP library through FastRPC.
The simulator executable `calculator_q` is not part of this deployment.

## Build inputs

In a terminal configured for the SDK:

```sh
cd /local/mnt/workspace/Qualcomm/Hexagon_SDK/6.6.0.0/examples/calculator
build_cmake ubuntuARM BUILD=Debug HLOS_ARCH=64 DOMAIN_FLAG=3
build_cmake hexagon BUILD=Debug DSP_ARCH=v68 NO_QURT_INC=1
```

The manifest uses absolute source paths under that SDK installation. Change
them if the SDK location, build variant, or tools version changes.

## Exact files and destinations

Source paths below are relative to the SDK's `examples/calculator/` directory.

| Local build artifact | Destination on the board | Role |
| --- | --- | --- |
| `UbuntuARM_Debug_aarch64/ship/calculator` | `/home/radxa/test/hexagon-calculator/bin/calculator` | ARM64 executable |
| `UbuntuARM_Debug_aarch64/ship/libcalculator.so` | `/home/radxa/test/hexagon-calculator/lib/libcalculator.so` | ARM64 FastRPC stub |
| `hexagon_Debug_toolv19_v68/ship/libcalculator_skel.so` | `/home/radxa/test/hexagon-calculator/dsp/libcalculator_skel.so` | Hexagon DSP skeleton and implementation |

The board already provides `/usr/lib/libcdsprpc.so`, `/dev/fastrpc-cdsp`, and
the firmware/runtime files under `/usr/lib/dsp/cdsp/`, including
`fastrpc_shell_unsigned_3`. The `radxa` user has access through the `render`
group. These were checked over SSH before testing this deployment. For a new
OS image, see [Radxa's FastRPC setup instructions](https://docs.radxa.com/en/dragon/q6a/app-dev/npu-dev/fastrpc-setup).

## Upload and run

From the repository root:

```sh
common/tools/remote-controller/remote-controller upload common/hardware/radxa_dragon_q6a/calculator/radxa_deployment_manifest.json
common/tools/remote-controller/remote-controller run common/hardware/radxa_dragon_q6a/calculator/radxa_deployment_manifest.json
```

Upload creates the destination directories and preserves executable permissions.
Run can be repeated without uploading. After rebuilding, upload again before
running. Neither command requires sudo or replaces system libraries.

The manifest launches `/usr/bin/env` to set library search paths for this process:

- `LD_LIBRARY_PATH` points to the uploaded ARM stub directory. The system loader
  finds the board's installed `libcdsprpc.so` through its normal system paths.
- `ADSP_LIBRARY_PATH` and `DSP_LIBRARY_PATH` include the uploaded DSP library
  directory first, then `/usr/lib/dsp/cdsp` and `/usr/lib/dsp` for the board's
  runtime files. DSP search entries are separated by semicolons.

`remote-controller` passes arguments literally. The `/home/radxa/...` paths in
`run.args` must be absolute; `~` and `$HOME` are not expanded there. If changing
the SSH user or deployment location, update both the upload destinations and
the paths in `run.args`.

## Change the input

The last arguments in the manifest are:

```json
"-r", "0",
"-d", "3",
"-U", "1",
"-n", "1000"
```

- `-r 0`: calculate on the DSP.
- `-d 3`: use CDSP.
- `-U 1`: use an unsigned protection domain.
- `-n 1000`: calculate over the numbers 0 through 999.

Change the value after `-n` and invoke `run` again; no rebuild or upload is needed.
For a CPU-only comparison, change `-r` to `1`.

## Verified result

The manifest was uploaded and run on the connected Ubuntu 24.04 ARM64 board.
The run returned exit status 0 and:

```text
Attempting to run on unsigned PD on domain 3
Compute sum on domain 3
Call calculator_sum on the DSP
Sum = 499500
Call calculator_max on the DSP
Max value = 999
Success
```

The manifest's `clean` operation removes only `~/test/hexagon-calculator`.
To remove this deployment explicitly:

```sh
common/tools/remote-controller/remote-controller clean common/hardware/radxa_dragon_q6a/calculator/radxa_deployment_manifest.json
```
