// Copyright (c) 2023 innodisk Crop.
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "hls_resize_normalize.hpp"

using namespace std;
using namespace cv;

int hls_resize_normalize::initial(const char* xclbin_path)
{
    xuid_t uuid;
    /* Open device and load xclbin, edge index 0 */
    dhdl = xrtDeviceOpen(0);
    auto xclbin = xrtXclbinAllocFilename(xclbin_path);
    if(xrtDeviceLoadXclbinHandle(dhdl, xclbin) != 0)
        return -1;

    /* Open kernel from device */
    xrtXclbinGetUUID(xclbin, uuid);
    khdl = xrtPLKernelOpen(dhdl, uuid, CU_NAME);

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
    /* Allocate Mat */
    img_out.create(out_height, out_width, CV_8UC3);
    /* Allocate XRT BO */
    bo_img_in = xrtBOAlloc(dhdl, size_img_in, 0, xrtKernelArgGroupId(khdl, 0));
    bo_img_out = xrtBOAlloc(dhdl, size_img_out, 0, xrtKernelArgGroupId(khdl, 1));
    /* Create runner */
    runner = xrtKernelRun(khdl, bo_img_in, bo_img_out, in_width, in_height, out_width, out_height);
    status = xrtRunWaitFor(runner, 1000);
}

Mat hls_resize::run(const Mat img_in, const unsigned int out_row, const unsigned int out_col)
{
    /* Update augments and reallocate buffer if image size changed */
    if(in_width != (unsigned int)img_in.rows || in_height != (unsigned int)img_in.cols || out_width != out_row || out_height != out_col)
        set(img_in.rows, img_in.cols, out_row, out_col);

    /* Write BO */
    xrtBOWrite(bo_img_in, img_in.data, size_img_in, 0);
    xrtBOSync(bo_img_in, XCL_BO_SYNC_BO_TO_DEVICE, size_img_in, 0);
    /* Execute runner */
    xrtRunStart(runner);
    /* Wait for runner, 1 sec timeout */
    status = xrtRunWaitFor(runner, 1000);
    /* Read BO */
    xrtBOSync(bo_img_out, XCL_BO_SYNC_BO_FROM_DEVICE, size_img_out, 0);
    xrtBORead(bo_img_out, img_out.data, size_img_out, 0);

    return img_out;
}

void hls_resize::release()
{
    xrtRunClose(runner);
    xrtBOFree(bo_img_in);
    xrtBOFree(bo_img_out);
    xrtKernelClose(khdl);
    xrtDeviceClose(dhdl);
}