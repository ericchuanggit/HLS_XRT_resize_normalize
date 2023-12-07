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

#include "common/xf_headers.hpp"
#include "xf_resize_normal_config.h"
#include <bitset>
#include <stdio.h>
using namespace std;

unsigned short in_width, in_height;
unsigned short out_width, out_height;

/****diff error****/
int diff(cv::Mat img_ocv, cv::Mat img_hls, const string& testcase){
    cv::Mat error;
    error.create(cv::Size(out_width, out_height), CV_8UC3);

    float err_per;
    cv::absdiff(img_ocv, img_hls, error);
    xf::cv::analyzeDiff(error, 5, err_per);
    cv::imwrite(testcase + "_" + "img_ocv.png", img_ocv);
    cv::imwrite(testcase + "_" + "img_hls.png", img_hls);
    cv::imwrite(testcase + "_" + "error.png", error);

    if (err_per > 0.0f) {
        // fprintf(stderr, "ERROR:" testcase + "_" + "Test Failed.\n");
        std::cerr << "ERROR:" << testcase << "_" << "test failed." << std::endl;
        return -1;
    }
    else{
        // fprintf(stdout, "Success:" testcase + "_" +  "Test matched.\n" );
        std::cout << "Success:" << testcase << "_" << "test matched." << std::endl;
    }
    return 0;
}
/***resize ***/
int t_resize(cv::Mat img){
    cv::Mat result_ocv, result_hls;
    result_ocv.create(cv::Size(out_width, out_height), CV_8UC3);
    result_hls.create(cv::Size(out_width, out_height), CV_8UC3);
#if INTERPOLATION == 0
    cv::resize(img, result_ocv, cv::Size(out_width, out_height), 0, 0, cv::INTER_NEAREST);
#endif
#if INTERPOLATION == 1
    cv::resize(img, result_ocv, cv::Size(out_width, out_height), 0, 0, cv::INTER_LINEAR);
#endif
#if INTERPOLATION == 2
    cv::resize(img, result_ocv, cv::Size(out_width, out_height), 0, 0, cv::INTER_AREA);
#endif

    float normalization[6] = {0, 0, 0, 1, 1, 1}; // resize only

    resize_accel((ap_uint<INPUT_PTR_WIDTH>*)img.data, (ap_uint<OUTPUT_PTR_WIDTH>*)result_hls.data, in_height, in_width,
                 out_height, out_width, normalization);

    return diff(result_ocv,result_hls,"resize");
}
/****alpha****/
int t_alpha(cv::Mat img){
    float alpha_value = 50;
    cv::Mat normalized_ocv, result_hls;
    normalized_ocv.create(cv::Size(out_width, out_height), CV_8UC3);
    result_hls.create(cv::Size(out_width, out_height), CV_8UC3);

    float hls_resize[6] = {0, 0, 0, 1, 1, 1}; // resize only
    
    resize_accel((ap_uint<INPUT_PTR_WIDTH>*)img.data, (ap_uint<OUTPUT_PTR_WIDTH>*)result_hls.data, in_height, in_width,
                 out_height, out_width, hls_resize);
    
    //opencv
    std::vector<cv::Mat> channels;
    cv::split(result_hls, channels);
    for(int i=0; i<3; i++){
        for (int row = 0; row < channels[i].rows; row++) {
            for (int col = 0; col < channels[i].cols; col++) {
                uchar pixel = channels[i].at<uchar>(row, col);

                if (pixel < alpha_value) {
                    channels[i].at<uchar>(row, col) = abs(int(pixel - alpha_value));
                    channels[i].at<uchar>(row, col) = ~channels[i].at<uchar>(row, col) + 1;
                } else {
                    channels[i].at<uchar>(row, col) = pixel - alpha_value;
                }
            }
        } 
    }
    cv::merge(channels,normalized_ocv);

    //hls
    float normalization[6] = {alpha_value, alpha_value, alpha_value, 1, 1, 1};

    resize_accel((ap_uint<INPUT_PTR_WIDTH>*)img.data, (ap_uint<OUTPUT_PTR_WIDTH>*)result_hls.data, in_height, in_width,
                 out_height, out_width, normalization);
    //diff
    return diff(normalized_ocv,result_hls,"alpha");
}
/****beta****/
int t_beta(cv::Mat img){
    cv::Mat normalized_ocv, result_hls;
    normalized_ocv.create(cv::Size(out_width, out_height), CV_8UC3);
    result_hls.create(cv::Size(out_width, out_height), CV_8UC3);

    float hls_resize[6] = {0, 0, 0, 1, 1, 1}; // resize only
    
    resize_accel((ap_uint<INPUT_PTR_WIDTH>*)img.data, (ap_uint<OUTPUT_PTR_WIDTH>*)result_hls.data, in_height, in_width,
                 out_height, out_width, hls_resize);

    //opencv
    float beta_value = 0.5;
    std::vector<cv::Mat> channels;
    cv::split(result_hls, channels);
    for(int i=0; i<3; i++){
        channels[i] = channels[i]*beta_value;
    }
    cv::merge(channels,normalized_ocv);

    //hls
    float normalization[6] = {0, 0, 0, beta_value, beta_value, beta_value};

    resize_accel((ap_uint<INPUT_PTR_WIDTH>*)img.data, (ap_uint<OUTPUT_PTR_WIDTH>*)result_hls.data, in_height, in_width,
                 out_height, out_width, normalization);
    //diff
    return diff(normalized_ocv,result_hls,"beta");

}

int main(int argc, char** argv){
    cv::Mat img;

    if (argc != 2) {
        fprintf(stderr, "Usage: <executable> <input image>\n");
        return -1;
    }
/*   Opencv image size  */
    img.create(cv::Size(WIDTH, HEIGHT), CV_8UC3);
    
/*   Reading mode in the color image   */
    img = cv::imread(argv[1], 1);

    if (!img.data) {
        return -1;
    }
/*   Writing the color image into ".png" */
    cv::imwrite("input.png", img);

    in_width = img.cols;
    in_height = img.rows;
    out_height = NEWHEIGHT;
    out_width = NEWWIDTH;

/*   Writing the color image into ".png" */
    t_resize(img);
    t_alpha(img);
    t_beta(img);

    return 0;
}