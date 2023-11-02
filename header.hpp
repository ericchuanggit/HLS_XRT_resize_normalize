// Copyright (c) 2023 innodisk Crop.
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef __HLS_IP__
#define __HLS_IP__

#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"
#include "xrt/xrt_bo.h"

#include "opencv2/opencv.hpp"

#define CU_NAME "resize_accel"

class hlsip{
    public:
    int create(const std::string xclbin);
    cv::Mat run(cv::Mat img_in, int out_width, int out_height, float* param);

    private:
    xrt::device device;
    xrt::kernel kernel;
    xrt::uuid uuid;
    xrt::run runner;
};

#endif /* __HLS_IP__ */