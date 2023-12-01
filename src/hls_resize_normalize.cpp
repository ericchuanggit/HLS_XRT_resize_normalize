// Copyright (c) 2023 innodisk Crop.
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "hls_resize_normalize.hpp"
#include "timer.hpp"

using namespace std;
using namespace cv;

int hls_resize_normalize::initial(const string xclbin_path)
{
    /* Open device and load xclbin, edge index 0 */
    device = xrt::device(0);

    uuid = device.load_xclbin(xclbin_path);

    kernel = xrt::kernel(device, uuid, CU_NAME);

    runner = xrt::run(kernel);

    bo_params     = xrt::bo(device, sizeof(float)*6, kernel.group_id(6));
    return 0;
}

void hls_resize_normalize::set(const unsigned int in_row, const unsigned int in_col, const unsigned int out_row, const unsigned int out_col)
{
    in_width = in_row;
    in_height = in_col;
    out_width = out_row;
    out_height = out_col;
    size_img_in = in_width*in_height*3*sizeof(unsigned char);
    size_img_out = out_width*out_height*3*sizeof(unsigned char);
    
    /* Allocate XRT BO */
    bo_img_in     = xrt::bo(device, size_img_in, xrt::bo::flags::normal, kernel.group_id(0));
    bo_img_out    = xrt::bo(device, size_img_out, xrt::bo::flags::cacheable, kernel.group_id(1));
}
    /* Allocate Mat */  
Mat hls_resize_normalize::run(const Mat img_in, const unsigned int out_row, const unsigned int out_col, const float* param)
{
    /* Update augments and reallocate buffer if image size changed */
    if(in_width != (unsigned int)img_in.rows || in_height != (unsigned int)img_in.cols || out_width != out_row || out_height != out_col)
        set(img_in.rows, img_in.cols, out_row, out_col);
    if(img_out.data)
        img_out.release();

    img_out.create(out_height, out_width, CV_8UC3);

    timer T;
    T.start();
    /* Write BO */
    bo_img_in.write(img_in.data);
    bo_img_in.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    bo_params.write(param);
    bo_params.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    T.end();


    /* Execute runner */
    runner(bo_img_in, bo_img_out, in_width, in_height, out_width, out_height, bo_params);
    /* Wait for runner, 1 sec timeout */
    status = runner.wait(1000);
    
    T.start();
    /* Read BO */
    bo_params.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    bo_img_out.read(img_out.data);
    T.end();
    
    return img_out;
}

void hls_resize_normalize::release()
{

}