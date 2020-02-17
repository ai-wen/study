#include <iostream>
 
#include <opencv2/opencv.hpp>
using namespace cv;
int main(int arg, char** args) {
	std::cout << "aa" << std::endl;
 
    std::string img = "D:\\1.JPG";
    Mat srcImage = imread(img);
    if (!srcImage.data) {
        return 1;
    }
    imshow("srcImage", srcImage);
    cvWaitKey(0);
    return 0;
}
