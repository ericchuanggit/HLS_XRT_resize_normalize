#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>


int main() {

    std::ofstream logFile("all_rgb.log");
    if (!logFile)
    {
        std::cerr << "Failed to open the log file!" << std::endl;
        return -1;
    }
    
    cv::Mat image = cv::imread("pic/error_alpha.png");
    if (image.empty()) {
        std::cerr << "Could not open or find the image!" << std::endl;
        return -1;
    }
    std::cout << "[catch] : all RGB value" << std::endl;
    for (int y = 0; y < image.rows; y++) {
        for (int x = 0; x < image.cols; x++) {
            cv::Vec3b color = image.at<cv::Vec3b>(y, x);

            uchar blue = color[0];
            uchar green = color[1];
            uchar red = color[2];

            logFile << "Coordinate (" << x << ", " << y << "): RGB(" << (int)red << ", " << (int)green << ", " << (int)blue << ")" << std::endl;
        }
    }

    logFile.close();

    std::cout << "[catch] : nonzero value" << std::endl; 
    

    std::ofstream NonZeroLog("nonzero_all.log");
    std::ofstream Log255("nonzero_255.log");

    if (!NonZeroLog || !Log255)
    {
        std::cerr << "Failed to open the log file!" << std::endl;
        return -1;
    }
    for (int y = 0; y < image.rows; y++) {
        for (int x = 0; x < image.cols; x++) {
            cv::Vec3b color = image.at<cv::Vec3b>(y, x);

            uchar blue = color[0];
            uchar green = color[1];
            uchar red = color[2];

            if((color[0] || color[1] || color[2]))
            {
                NonZeroLog << "Coordinate (" << x << ", " << y << "): RGB(" << (int)red << ", " << (int)green << ", " << (int)blue << ")" << std::endl;
            }

            if(((color[0]==255) || (color[1]==255) || (color[2]==255)))
            {
                Log255 << "Coordinate (" << x << ", " << y << "): RGB(" << (int)red << ", " << (int)green << ", " << (int)blue << ")" << std::endl;
            }
        }
    }

    NonZeroLog.close();
    Log255.close();
    
    std::cout << "[catch done]" << std::endl;  

    return 0;
}
