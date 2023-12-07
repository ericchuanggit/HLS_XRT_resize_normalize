/*
 * Copyright 2019 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define RO 0 // Resource Optimized (8-pixel implementation)
#define NO 1 // Normal Operation (1-pixel implementation)

// port widths
#define INPUT_PTR_WIDTH 128
#define OUTPUT_PTR_WIDTH 128

// For Nearest Neighbor & Bilinear Interpolation, max down scale factor 2 for all 1-pixel modes, and for upscale in x
// direction
#define MAXDOWNSCALE 2

#define RGB 1
#define GRAY 0
/* Interpolation type*/
#define INTERPOLATION 1
// 0 - Nearest Neighbor Interpolation
// 1 - Bilinear Interpolation
// 2 - AREA Interpolation

/* Input image Dimensions */
#define WIDTH 1920  // Maximum Input image width
#define HEIGHT 1080 // Maximum Input image height

// static constexpr int WIDTH = 1920;
// static constexpr int HEIGHT = 1080;

/* Output image Dimensions */
#define NEWWIDTH 720  // Maximum output image width
#define NEWHEIGHT 720 // Maximum output image height
#define XF_CV_DEPTH_IN 3
#define XF_CV_DEPTH_OUT 3

// preprocess kernel params out = (in - a) * b
// a, b and out are fixed point values and below params are used to configure
// the width and integer bits
#define WIDTH_A 8
#define IBITS_A 8
#define WIDTH_B 8
#define IBITS_B 4 // so B is 8-bit wide and 4-bits are integer bits
#define WIDTH_OUT 8
#define IBITS_OUT 8

//test RGB/Alpha/Beta
#define Beta_r 0
#define Beta_g 0
#define Beta_b 0
#define Alpha_r 0
#define Alpha_g 0
#define Alpha_b 0
#define Alpha_bgr 1
