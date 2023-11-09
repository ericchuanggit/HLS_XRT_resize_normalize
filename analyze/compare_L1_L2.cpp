#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;

int main (int argc, char* argv[]) {

    if(argc != 3)
    {
        std::cout << "Usage : <exec> <L1 image path> <L2 image path>" << std::endl;
        std::cout << " <exec> = ./compare "<< std::endl;
        return -1;
    }

    std::cout << "[catch L1 image]" << std::endl;
    Mat imageL1 = imread(argv[1]);
    if (imageL1.empty()) {
        std::cerr << "Could not open or find the image!" << std::endl;
        return -1;
    }
    std::cout << "[catch L2 image]" << std::endl;
    Mat imageL2 = imread(argv[2]);
    if (imageL2.empty()) {
        std::cerr << "Could not open or find the image!" << std::endl;
        return -1;
    }


    std::cout << "[Output] : error.png" << std::endl;
    Mat error_img;
    absdiff(imageL1 ,imageL2 ,error_img);

    imwrite("error.png",error_img);

    return 0;
}

