// Copyright (c) 2023 innodisk Crop.
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <string>
#include <unistd.h>
#include "hls_resize_normalize.hpp"

using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        cout << "Usage : <exec> <xclbin> <input image>" << endl;
        return -1;
    }

    Mat img_in, img_out, img_out2, img_out3;
    img_in = imread(argv[2]);
    imwrite("tmp_in.png", img_in);

    unsigned int out_cols = 416;
    unsigned int out_rows = 416;



    cout << "hls api first call" << endl;

    hls_resize_normalize ip;
    ip.initial(argv[1]);

    clock_t t0 = clock();
    float param[6]={0,0,0,1,1,1};
    img_out = ip.run(img_in, out_rows, out_cols, param);
    cout << float(clock() - t0) / CLOCKS_PER_SEC << endl << endl;

    imwrite("out_run.png", img_out);

    // cout << "hls api second call" << endl;
    // cout << "[out_run2.png]" << endl;

    // clock_t t1 = clock();
    // param[3]=1;
    // param[4]=1;
    // param[5]=1;
    // param[0]=50;
    // img_out2 = ip.run(img_in, out_rows, out_cols, param);
    // cout << float(clock() - t1) / CLOCKS_PER_SEC << endl << endl;
    // imwrite("out_run2.png", img_out2);

    // cout << "param[0] = " << param[0] <<endl;
    // cout << "param[1] = " << param[1] <<endl;
    // cout << "param[2] = " << param[2] <<endl;
    // cout << "param[3] = " << param[3] <<endl;
    // cout << "param[4] = " << param[4] <<endl;
    // cout << "param[5] = " << param[5] <<endl;

    // cout << "[out_run3.png]" << endl;

    // clock_t t2 = clock();
    // param[0]=0;
    // param[1]=50;
    // img_out2 = ip.run(img_in, out_rows, out_cols, param);
    // cout << float(clock() - t2) / CLOCKS_PER_SEC << endl << endl;
    // imwrite("out_run3.png", img_out2);


    // cout << "param[0] = " << param[0] <<endl;
    // cout << "param[1] = " << param[1] <<endl;
    // cout << "param[2] = " << param[2] <<endl;
    // cout << "param[3] = " << param[3] <<endl;
    // cout << "param[4] = " << param[4] <<endl;
    // cout << "param[5] = " << param[5] <<endl;
    
    // cout << "[out_run4.png]" << endl;

    // clock_t t3 = clock();
    // param[1]=0;
    // param[2]=50;
    // img_out2 = ip.run(img_in, out_rows, out_cols, param);
    // cout << float(clock() - t3) / CLOCKS_PER_SEC << endl << endl;
    // imwrite("out_run4.png", img_out2);


    // cout << "param[0] = " << param[0] <<endl;
    // cout << "param[1] = " << param[1] <<endl;
    // cout << "param[2] = " << param[2] <<endl;
    // cout << "param[3] = " << param[3] <<endl;
    // cout << "param[4] = " << param[4] <<endl;
    // cout << "param[5] = " << param[5] <<endl;
    
    // cout << "[out_run5.png]" << endl;

    // clock_t t4 = clock();
    // param[2]=0;
    // param[3]=0.5;
    // img_out2 = ip.run(img_in, out_rows, out_cols, param);
    // cout << float(clock() - t4) / CLOCKS_PER_SEC << endl << endl;
    // imwrite("out_run5.png", img_out2);


    // cout << "param[0] = " << param[0] <<endl;
    // cout << "param[1] = " << param[1] <<endl;
    // cout << "param[2] = " << param[2] <<endl;
    // cout << "param[3] = " << param[3] <<endl;
    // cout << "param[4] = " << param[4] <<endl;
    // cout << "param[5] = " << param[5] <<endl;

    // cout << "[out_run6.png]" << endl;

    // clock_t t5 = clock();
    // param[0]=0;
    // param[1]=0;
    // param[2]=0;
    // param[3]=0.5;
    // param[4]=0.5;
    // param[5]=0.5;
    // img_out2 = ip.run(img_in, out_rows, out_cols, param);
    // cout << float(clock() - t5) / CLOCKS_PER_SEC << endl << endl;
    // imwrite("out_run6.png", img_out2);

    // cout << "param[0] = " << param[0] <<endl;
    // cout << "param[1] = " << param[1] <<endl;
    // cout << "param[2] = " << param[2] <<endl;
    // cout << "param[3] = " << param[3] <<endl;
    // cout << "param[4] = " << param[4] <<endl;
    // cout << "param[5] = " << param[5] <<endl;

    ip.release();
    //opencv alpha
    cout << "sw" << endl;

    clock_t t6 = clock();
    
    resize(img_in, img_out3, Size(out_cols, out_rows), 0, 0, INTER_LINEAR);
    float alpha_value = 50;
    vector<Mat> channels_alpha;
    split(img_out3, channels_alpha);
    for(int i=0; i<3; i++){
        for (int row = 0; row < channels_alpha[i].rows; row++) {
            for (int col = 0; col < channels_alpha[i].cols; col++) {
                uchar pixel = channels_alpha[i].at<uchar>(row, col);

                if (pixel < alpha_value) {
                    channels_alpha[i].at<uchar>(row, col) = abs(int(pixel - alpha_value));
                    channels_alpha[i].at<uchar>(row, col) = ~channels_alpha[i].at<uchar>(row, col) + 1;
                } else {
                    channels_alpha[i].at<uchar>(row, col) = pixel - alpha_value;
                }
            }
        } 
    }
    Mat normalized_ocv_a;
    cv::merge(channels_alpha,normalized_ocv_a);

    cout << float(clock() - t6) / CLOCKS_PER_SEC << endl << endl;

    imwrite("out_run_ocv_alpha.png", normalized_ocv_a);


    //opencv beta
    cout << "sw" << endl;

    clock_t t7 = clock();
    
    resize(img_in, img_out3, Size(out_cols, out_rows), 0, 0, INTER_LINEAR);
    float beta_value = 0.5;
    vector<Mat> channels_beta;
    split(img_out3, channels_beta);
    for(int i=0; i<3; i++){
        channels_beta[i] = channels_beta[i]*beta_value;
    }

    Mat normalized_ocv_b;
    cv::merge(channels_beta,normalized_ocv_b);

    cout << float(clock() - t7) / CLOCKS_PER_SEC << endl << endl;

    imwrite("out_run_ocv_beta.png", normalized_ocv_b);

    return 0;
};
