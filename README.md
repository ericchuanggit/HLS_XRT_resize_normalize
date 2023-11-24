<!--
 Copyright (c) 2023 innodisk Crop.
 
 This software is released under the MIT License.
 https://opensource.org/licenses/MIT
-->
- [Overview](#overview)
- [Requirement](#requirement)
  - [Software](#software)
  - [Firmware](#firmware)
  - [Hardware](#hardware)
- [How to build](#how-to-build)
- [How to use](#how-to-use)
- [How to run](#how-to-run)
- [Notification](#notification)
- [Reference](#reference)

# Overview
This repository shows the example to control HLS resize_normalize IP by using XRT API.
# Requirement
## Software
- xrt 2.13.0
- opencv 4.5.2
## Firmware
- xclbin (This file will be generate from Vitis flow.)
- loaded bitstream which with resize accel
## Hardware
- Xilinx FPGA - zynqmp

# How to build
- Install meson and ninja.
  ```bash
  pip3 install meson ninja
  ```
- Build the libraries and binary file
  ```bash
  soruce <xilinx-sdk>
  ./build.sh
  ```

# How to use
- Following shows how to use this api proiperly.
  ```cpp
  /* declared instance */
  hls_resize_normalize ip;
  /* initial instance */
  ip.initial("Path to xclbin file");
  /* execute instance */
  Mat img_out0 = ip.run(img_in0, out_rows, out_cols, param);
  ...
  Mat img_outn = ip.run(img_inn, out_rows, out_cols, param);
  /* release instance */
  ip.release();
  ```

# How to run
- The result should be like :
  ```bash
  ./main app-eric-1019.xclbin input.png 
  -------------------------------------------------
  hls api resize & normalize first call
  0.0195293
  -------------------------------------------------
  hls api resize & normalize second call
  0.0178829
  -------------------------------------------------
  cpu call
  0.0143743
  -------------------------------------------------
  ```
- Experimental environment
  - Xilinx k26 platform
  - HLS IP Clock : 275MHz
  - HLS IP Info. : resize NPPC1

# How to install
- Default install to `/usr/lib`, if using sdk it will install into the sdk.
  ```bash
  # Run build.sh before this executing the install script.
  ./install.sh
  ``` 

# Notification
- If the input or output image size changed the buffers will be reallocate, else will using the original buffer. So as you can see in [this section](#how-to-run) the second execution of `hls_resize_normalize::run` takes only 90%.
- The maximun value of input and output image size is declared in hw ip.
- The most of time taked in `hls_resize_normalize::run` is writing the `BO` buffer, the acctual calculation time is veryshort.
- Althought HLS IP takes more time then traditional opencv, it still makes the CPU could do something more important that enhance the overall preformance.


# Reference
- [Vitis Library 2022.1](https://github.com/Xilinx/Vitis_Libraries)
- [Xilinx Runtime](https://github.com/Xilinx/XRT)

