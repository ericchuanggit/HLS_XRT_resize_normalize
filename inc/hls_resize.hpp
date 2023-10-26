// Copyright (c) 2023 innodisk Crop.
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef __HLS_IP__
#define __HLS_IP__

#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"
#include "xrt/xrt_bo.h"
#include "xrt/xrt.h"
#include "opencv2/opencv.hpp"

using namespace cv;

#define CU_NAME "resize_accel"

class hls_resize{
    public:
    int initial(const char* xclbin_path);
    Mat run(const Mat img_in, const unsigned int out_row, const unsigned int out_col);
    void release();
    int status; // this status from "ert.h" shows the runner wait status of previous run
        /*
        ERT_CMD_STATE_NEW = 1,
        ERT_CMD_STATE_QUEUED = 2,
        ERT_CMD_STATE_RUNNING = 3,
        ERT_CMD_STATE_COMPLETED = 4,
        ERT_CMD_STATE_ERROR = 5,
        ERT_CMD_STATE_ABORT = 6,
        ERT_CMD_STATE_SUBMITTED = 7,
        ERT_CMD_STATE_TIMEOUT = 8,
        ERT_CMD_STATE_NORESPONSE = 9,
        ERT_CMD_STATE_SKERROR = 10, //Check for error return code from Soft Kernel
        ERT_CMD_STATE_SKCRASHED = 11, //Soft kernel has crashed
        ERT_CMD_STATE_MAX, // Always the last one
        */
    private:
    void set(const unsigned int in_row, const unsigned int in_col, const unsigned int out_row, const unsigned int out_col);
    xrtDeviceHandle dhdl; // device handle
    xrtKernelHandle khdl; // kernel handle
    xrtRunHandle runner; // runner handle
    xrtBufferHandle bo_img_in; // input image BO
    xrtBufferHandle bo_img_out; // output image BO
    unsigned int in_width;
    unsigned int in_height;
    unsigned int out_width;
    unsigned int out_height;
    size_t size_img_in;
    size_t size_img_out;
    Mat img_out;
};

#endif /* __HLS_IP__ */