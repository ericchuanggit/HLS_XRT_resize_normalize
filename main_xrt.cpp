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
    cout << "-------------------------------------------------" << endl;
    cout << "hls api first call resize<416x416>" << endl;
    

    hls_resize_normalize ip;
    ip.initial(argv[1]);

    cout << "[out_run_resize.png]" << endl;
    clock_t t0 = clock();
    /***   normaliztion -> (pixel - alpha) * beta   ***/
    /* hls reisze & normalize */
    float param[6]={0,0,0,1,1,1}; // param[0][1][2] -> alpha , param[3][4][5] -> beta , resize only
    img_out = ip.run(img_in, out_rows, out_cols, param);
    cout << float(clock() - t0) / CLOCKS_PER_SEC << endl;
    imwrite("out_run_resize.png", img_out);
    cout << "-------------------------------------------------" << endl;
    cout << "hls api second call normalize<(pixel-alpha)*beta>" << endl;

    //normalize_hls alpha
    float alpha_value = 50;
    cout << "[alpha_value = 50]" << endl;
    clock_t t1 = clock();

    param[0]=alpha_value;
    param[1]=alpha_value;
    param[2]=alpha_value;
    param[3]=1;
    param[4]=1;
    param[5]=1;

    img_out_alpha = ip.run(img_in, out_rows, out_cols, param);
    cout << "[out_run_alpha.png]" << endl;
    cout << float(clock() - t1) / CLOCKS_PER_SEC << endl << endl;
    imwrite("out_run_alpha.png", img_out_alpha);

    //normalize_hls beta
    float beta_value = 0.5;
    cout << "[beta_value = 0.5]" << endl;
    clock_t t2 = clock();
    
    param[0]=0;
    param[1]=0;
    param[2]=0;
    param[3]=beta_value;
    param[4]=beta_value;
    param[5]=beta_value;
    
    img_out_beta = ip.run(img_in, out_rows, out_cols, param);
    cout << "[out_run_beta.png]" << endl;
    cout << float(clock() - t2) / CLOCKS_PER_SEC << endl;
    imwrite("out_run_beta.png", img_out_beta);

    ip.release();

    /* opencv alpha software compare test */
    cout << "-------------------------------------------------" << endl;
    cout << "sw : opencv alpha" << endl;
    Mat img_out_ocv_a;
    clock_t t3 = clock();

    //from hls resize
    param[0]=0;
    param[1]=0;
    param[2]=0;
    param[3]=1;
    param[4]=1;
    param[5]=1;
    img_out_ocv_a = ip.run(img_in, out_rows, out_cols, param);

    //normalize_ocv alpha
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


    /* opencv beta software compare test */
    cout << "sw :opencv beta" << endl;
    

    Mat img_out_ocv_b;
    clock_t t4 = clock();
    
    //from hls resize
    param[0]=0;
    param[1]=0;
    param[2]=0;
    param[3]=1;
    param[4]=1;
    param[5]=1;
    img_out_ocv_b = ip.run(img_in, out_rows, out_cols, param);

    //normalize_ocv beta
    vector<Mat> channels_beta;
    split(img_out_ocv_b, channels_beta);
    for(int i=0; i<3; i++){
        channels_beta[i] = channels_beta[i]*beta_value;
    }
    Mat normalized_ocv_b; 
    cv::merge(channels_beta,normalized_ocv_b);

    cout << float(clock() - t4) / CLOCKS_PER_SEC << endl;
    cout << "-------------------------------------------------" << endl;

    imwrite("out_run_ocv_beta.png", normalized_ocv_b);

    /****diff error****/
    Mat error_a ,error_b;
    absdiff(normalized_ocv_a, img_out_alpha, error_a);
    absdiff(normalized_ocv_b, img_out_beta, error_b);
    
    imwrite("error_alpha.png" ,error_a);
    imwrite("error_beta.png" ,error_b);
    
    return 0;
};
