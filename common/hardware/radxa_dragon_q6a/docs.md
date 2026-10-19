# Radxa Dragon Q6A Documentation

The official Radxa Dragon Q6A documentation is available in the [Radxa documentation](https://docs.radxa.com/en/dragon/q6a).

The QCS6490 and QCS5430 data sheet is included in this directory as `QCS6490_and_QCS5430_Data_Sheet.pdf`.

## Setup

1. Flash Radxa OS to an SD card and insert the card into the board's SD card slot.
2. Attach the Radxa Heatsink 2012B to the SoC. The heatsink is slightly oversized, but it fits the board. See the [Radxa product page](https://radxa.com/products/accessories/heatsink-2012b/).
3. Connect an Ethernet cable for headless operation.
4. Connect a 67 W USB-C phone charger to power the board.

## Hexagon setup

1. Open the [Qualcomm Software Center catalog](https://softwarecenter.qualcomm.com/catalog?index=2) and download Qualcomm Software Center.
2. Create a Qualcomm account, then sign in to Qualcomm Software Center.
3. Find the Hexagon SDK version you need, then download and install it.

I installed the SDK at `/local/mnt/workspace/Qualcomm/Hexagon_SDK/6.6.0.0/`.
For detailed instructions on setting up the development environment, see the
`README.md` in the Hexagon SDK root directory.

Added following to `~/.bashrc` for convenience:
```
source /local/mnt/workspace/Qualcomm/Hexagon_SDK/6.6.0.0/setup_sdk_env.source
export HEXAGON_TOOLS_ROOT=/local/mnt/workspace/Qualcomm/Hexagon_SDK/6.6.0.0/tools/HEXAGON_Tools/19.0.07
export PATH=$HEXAGON_TOOLS_ROOT/Tools/bin:$PATH
```

## Deploy the calculator example over SSH

See [the calculator deployment guide](calculator/README.md) for the exact ARM
and DSP artifacts, their destinations, and the manifest-based upload/run commands.
The manifest uses the board's installed FastRPC runtime and deploys the example
under `~/test/hexagon-calculator`.
