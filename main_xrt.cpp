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

    Mat img_in, img_out, img_out_alpha, img_out_beta;
    img_in = imread(argv[2]);
    imwrite("tmp_in.png", img_in);

    unsigned int out_cols = 416;
    unsigned int out_rows = 416;
    
    // Mat img_resize;
    // resize(img_in, img_resize, Size(1080, 1080), 0, 0, INTER_LINEAR);
    // imwrite("resize.png", img_resize);

    cout << "hls api first call" << endl;
    

    hls_resize_normalize ip;
    ip.initial(argv[1]);

    cout << "[out_run.png]" << endl;
    clock_t t0 = clock();
    /***   normaliztion -> (pixel - alpha) * beta   ***/
    float param[6]={0,0,0,1,1,1}; // param[0][1][2] -> alpha , param[3][4][5] -> beta
    img_out = ip.run(img_in, out_rows, out_cols, param);
    cout << float(clock() - t0) / CLOCKS_PER_SEC << endl << endl;
    imwrite("out_run.png", img_out);

    cout << "hls api second call" << endl;
    cout << "[out_run_alpha.png]" << endl;
    clock_t t1 = clock();

    param[0]=50;
    param[1]=50;
    param[2]=50;
    param[3]=1;
    param[4]=1;
    param[5]=1;

    img_out_alpha = ip.run(img_in, out_rows, out_cols, param);
    cout << float(clock() - t1) / CLOCKS_PER_SEC << endl;
    imwrite("out_run_alpha.png", img_out_alpha);


    cout << "[out_run_beta.png]" << endl;
    clock_t t2 = clock();
    
    param[0]=0;
    param[1]=0;
    param[2]=0;
    param[3]=0.5;
    param[4]=0.5;
    param[5]=0.5;
    
    img_out_beta = ip.run(img_in, out_rows, out_cols, param);
    cout << float(clock() - t2) / CLOCKS_PER_SEC << endl << endl;
    imwrite("out_run_beta.png", img_out_beta);

    ip.release();

    //opencv alpha
    cout << "sw alpha" << endl;
    Mat img_out_ocv_a;
    clock_t t3 = clock();
    
    resize(img_in, img_out_ocv_a, Size(out_cols, out_rows), 0, 0, INTER_LINEAR);
    float alpha_value = 50;
    vector<Mat> channels_alpha;
    split(img_out_ocv_a, channels_alpha);
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

    cout << float(clock() - t3) / CLOCKS_PER_SEC << endl;

    imwrite("out_run_ocv_alpha.png", normalized_ocv_a);


    //opencv beta
    cout << "sw beta" << endl;

    Mat img_out_ocv_b;
    clock_t t4 = clock();
    
    resize(img_in, img_out_ocv_b, Size(out_cols, out_rows), 0, 0, INTER_LINEAR);
    float beta_value = 0.5;
    vector<Mat> channels_beta;
    split(img_out_ocv_b, channels_beta);
    for(int i=0; i<3; i++){
        channels_beta[i] = channels_beta[i]*beta_value;
    }
    Mat normalized_ocv_b; 
    cv::merge(channels_beta,normalized_ocv_b);

    cout << float(clock() - t4) / CLOCKS_PER_SEC << endl << endl;

    imwrite("out_run_ocv_beta.png", normalized_ocv_b);

    /****diff error****/
    Mat error_a ,error_b;
    absdiff(normalized_ocv_a, img_out_alpha, error_a);
    absdiff(normalized_ocv_b, img_out_beta, error_b);
    
    // float err_per_a,err_per_b;
    // xf::cv::analyzeDiff(error_a, 5, err_per_a);
    // xf::cv::analyzeDiff(error_b, 5, err_per_b);
    // if ((err_per_a > 0.0f) && (err_per_b > 0.0f){
    //         std::cerr << "ERROR: test failed." << std::endl;
    //         return -1;
    //     }
    //     else{
    //         std::cout << "Success: test matched." << std::endl;
    //     }

    imwrite("error_alpha.png" ,error_a);
    imwrite("error_beta.png" ,error_b);
    
    return 0;
};
