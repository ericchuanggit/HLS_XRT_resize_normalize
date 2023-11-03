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
This repository shows the example to control HLS resize + normalize IP by using XRT API.
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
  Mat img_out_alpha = ip.run(img_in, out_rows, out_cols, param);
  ...
  Mat img_out_beta = ip.run(img_in, out_rows, out_cols, param);
  /* release instance */
  ip.release();
  ```

# How to run
- The result should be like :
  ```bash
  # ./main_xrt app-eric-1019.xclbin input.jpg 
  hls api first call
  [out_run.png]
  0.007776

  hls api second call
  [out_run_alpha.png]
  0.006114
  [out_run_beta.png]
  0.005835

  sw alpha
  0.029339
  sw beta
  0.015208
  
  ```
- Experimental environment
  - Xilinx k26 platform
  - HLS IP Clock : 275MHz
  - HLS IP Info. : resize NPPC1
# Notification
- If the input or output image size changed the buffers will be reallocate, else will using the original buffer. So as you can see in [this section](#how-to-run) the second execution of `hls_resize_normalize::run` takes only 60%.
- The maximun value of input and output image size is declared in hw ip.
- The most of time taked in `hls_resize_normalize::run` is writing the `BO` buffer, the acctual calculation time is veryshort.

# Reference
- [Vitis Library 2022.1](https://github.com/Xilinx/Vitis_Libraries)
- [Xilinx Runtime](https://github.com/Xilinx/XRT)

