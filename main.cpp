// Copyright (c) 2023 innodisk Crop.
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <string>
#include <unistd.h>
#include "hls_resize_normalize.hpp"
#include "timer.hpp"
/***   normaliztion -> (pixel - alpha) * beta   ***/


using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        cout << "Usage : <exec> <xclbin> <input image>" << endl;
        return -1;
    }

    Mat img_in, img_hls;
    img_in = imread(argv[2]);
    imwrite("tmp_in.png", img_in);
    
    timer T;

    unsigned int out_cols = 416;
    unsigned int out_rows = 416;


    cout << "-------------------------------------------------" << endl;
    cout << "hls api resize & normalize first call" << endl;

    hls_resize_normalize ip;
    ip.initial(argv[1]);
  
    /* hls reisze & normalize */
    float param[6]={0,0,0,1,1,1}; // param[0][1][2] -> alpha , param[3][4][5] -> beta , resize only
    
    T.start();
    img_hls = ip.run(img_in, out_rows, out_cols, param);
    T.end();

    imwrite("hls_first.png", img_hls);

    cout << "-------------------------------------------------" << endl;

    cout << "hls api resize & normalize second call" << endl;
    T.start();
    img_hls = ip.run(img_in, out_rows, out_cols, param);
    T.end();

    imwrite("hls_second.png", img_hls);

    ip.release();
    cout << "-------------------------------------------------" << endl; 
    /* opencv software cpu test*/
    cout << "cpu call" << endl;

    T.start();
    Mat img_opencv;
    resize(img_in, img_opencv, Size(out_rows, out_cols), 0, 0, INTER_LINEAR);
    //opencv normalize
    vector<Mat> channels;
    split(img_opencv, channels);

    channels[0] = (channels[0] - param[0]) * param[3];
    channels[1] = (channels[1] - param[1]) * param[4];
    channels[2] = (channels[2] - param[2]) * param[5];

    cv::merge(channels,img_opencv);
    T.end();
 
    imwrite("cpu.png", img_opencv);
    cout << "-------------------------------------------------" << endl;


    /****diff error****/
    Mat error;
    absdiff(img_opencv, img_hls, error);
    imwrite("error.png" ,error);
 
    return 0;
};
