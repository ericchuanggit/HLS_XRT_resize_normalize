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
    cout << float(clock() - t0) / CLOCKS_PER_SEC << endl;

    imwrite("out_run.png", img_out);

    cout << "hls api second call" << endl;

    clock_t t1 = clock();
    img_out2 = ip.run(img_in, out_rows, out_cols, param);
    cout << float(clock() - t1) / CLOCKS_PER_SEC << endl;

    imwrite("out_run2.png", img_out2);

    ip.release();

    //opencv
    cout << "sw" << endl;

    clock_t t2 = clock();
    
    resize(img_in, img_out3, Size(out_cols, out_rows), 0, 0, INTER_LINEAR);
    int alpha_value = 0;
    vector<Mat> channels;
    split(img_out3, channels);
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
    Mat normalized_ocv;
    cv::merge(channels,normalized_ocv);

    cout << float(clock() - t2) / CLOCKS_PER_SEC << endl;

    imwrite("out_run3.png", normalized_ocv);

    return 0;
};
