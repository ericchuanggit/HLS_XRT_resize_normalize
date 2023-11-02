#include <string>
#include "header.hpp"

int hlsip::create(const std::string xclbin)
{
    std::cout << "[INFO] enter create\n";
    if(xclbin.empty())
        return -1;

    device = xrt::device(0);
    uuid = device.load_xclbin(xclbin);
    kernel = xrt::kernel(device, uuid, CU_NAME);
    runner = xrt::run(kernel);

    return 0;
};

cv::Mat hlsip::run(cv::Mat img_in, int out_width, int out_height, float* param)
{
    std::cout << "[INFO] enter run\n";
    cv::Mat img_out;
    int in_width = img_in.cols;
    int in_height = img_in.rows;

    size_t size_img_in = in_width*in_height*3*sizeof(unsigned char);
    size_t size_img_out = out_width*out_height*3*sizeof(unsigned char);
    std::cout << "[OUT] size_img_in :" << size_img_in << std::endl;
    std::cout << "[OUT] size_img_out :" << size_img_out << std::endl;

    img_in.create(in_height, in_width, CV_8UC3);
    img_out.create(out_height, out_width, CV_8UC3);

    std::cout << "[INFO] allocate buffer\n";
    // allocate buffer to 10 augments for HLS IP
    auto bo_img_in     = xrt::bo(device, size_img_in, kernel.group_id(0));
    auto bo_img_out    = xrt::bo(device, size_img_out, kernel.group_id(1));
    auto bo_params     = xrt::bo(device, sizeof(float)*6, kernel.group_id(6));

    std::cout << "[INFO] write buffer\n";
    // write data to buffer
    bo_img_in.write(img_in.data);
    bo_img_in.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    bo_params.write(param);
    bo_params.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    std::cout << "[INFO] exec runner\n";
    runner(bo_img_in, bo_img_out, in_height, in_width, out_height, out_width, bo_params);
    
    std::cout << "[INFO] waiting kernel" << std::endl;
    int status = runner.wait(10000);
    std::cout << "[INFO] runner status : " << status << std::endl;

    std::cout << "[INFO] get data from IP to buffer" << std::endl;
    
    bo_params.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    bo_img_out.read(img_out.data);
    
    cv::imwrite("img_out.png", img_out);

    return img_out;
};
static void usage()
{
    std::cout << "[INFO] usage: <bin> <xclbin> <image>" << std::endl;
    std::cout << "[INFO] default cu_name : " << CU_NAME << std::endl;
};


int main(int argc, char* argv[])
{
    if(argc < 3){
        usage();
        return 1;
    }

    cv::Mat img_in;
    float param[6]={0,0,0,1,1,1};
    std::string xclbin_fnm = argv[1];

    if (xclbin_fnm.empty())
        throw std::runtime_error("FAILED_TEST\nNo xclbin specified");

    std::cout << "[INFO] imread\n";
    img_in = cv::imread(argv[2],1);

    if (!img_in.data) {
        return 2;
    }

    // declare instance
    hlsip ip;

    // initial instance
    std::cout << "[INFO] initial instance" << std::endl;
    if(ip.create(xclbin_fnm) != 0){
        std::cout << "[INFO] initial failed" << std::endl;
        return 3;
    }
    // excute instance
    std::cout << "[INFO] excute instance" << std::endl;
    cv::Mat img_result = ip.run(img_in, 416, 416, param);

    return 0;
}

