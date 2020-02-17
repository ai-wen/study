#include <iostream>

#include <opencv2/opencv.hpp>
using namespace cv;
int main1(int arg, char **args)
{
    std::cout << "aa" << std::endl;

    std::string img = "D:\\1.JPG";
    Mat srcImage = imread(img);
    if (!srcImage.data)
    {
        return 1;
    }
    imshow("srcImage", srcImage);
    cvWaitKey(0);
    return 0;
}



int main(int arc, char **argv)
{

    Mat src = imread("1.jpg");
    namedWindow("input", CV_WINDOW_AUTOSIZE);
    imshow("input", src);
    //组装数据并运行KMeans
    int width = src.cols;
    int height = src.rows;
    int dims = src.channels();
    int pointsCount = width * height;
    Mat points(pointsCount, dims, CV_32F); //kmeans要求的数据为float类型的
    int index = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            index = i * width + j;
            points.at<float>(index, 0) = src.at<Vec3b>(i, j)[0];
            points.at<float>(index, 1) = src.at<Vec3b>(i, j)[1];
            points.at<float>(index, 2) = src.at<Vec3b>(i, j)[2];
        }
    }

    Mat bestLabels;
    Mat centers;
    kmeans(points, 4, bestLabels, TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 10, 0.1), 3, 2, centers);

    //去背景+遮罩层
    Mat mask(src.size(), CV_8UC1);
    index = src.cols * 2 + 2;
    int bindex = bestLabels.at<int>(index, 0); //获得kmeans后背景的标签

    Mat dst;
    src.copyTo(dst);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            index = i * width + j;
            int label = bestLabels.at<int>(index, 0);
            if (label == bindex)
            {
                dst.at<Vec3b>(i, j)[0] = 0;
                dst.at<Vec3b>(i, j)[1] = 0;
                dst.at<Vec3b>(i, j)[2] = 0;
                mask.at<uchar>(i, j) = 0;
            }
            else
            {
                mask.at<uchar>(i, j) = 255;
            }
        }
    }

    imshow("mask", mask);
    imshow("kmeans", dst);

    //对掩码进行腐蚀+高斯模糊
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    erode(mask, mask, kernel);
    imshow("erode mask", mask);
    GaussianBlur(mask, mask, Size(3, 3), 0, 0);
    imshow("blur mask", mask);

    //通道混合
    Vec3b color;
    color[0] = theRNG().uniform(0, 255);
    color[1] = theRNG().uniform(0, 255);
    color[2] = theRNG().uniform(0, 255);

    Mat result(src.size(), src.type());
    double w = 0.0;
    int b = 0, g = 0, r = 0;
    int b1 = 0, g1 = 0, r1 = 0;
    int b2 = 0, g2 = 0, r2 = 0;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int m = mask.at<uchar>(i, j);
            if (m == 255)
            {
                result.at<Vec3b>(i, j) = src.at<Vec3b>(i, j); //将原图像中前景赋给结果图像中的前景
            }
            else if (m == 0)
            {
                result.at<Vec3b>(i, j) = color; //将随机生成的颜色赋给结果图像中的背景
            }
            else
            {
                w = m / 255.0; //权重
                //边缘前景
                b1 = src.at<Vec3b>(i, j)[0];
                g1 = src.at<Vec3b>(i, j)[1];
                r1 = src.at<Vec3b>(i, j)[2];
                //边缘背景
                b2 = color[0];
                g2 = color[1];
                r2 = color[2];
                //边缘融合
                b = b1 * w + b2 * (1.0 - w);
                g = g1 * w + g2 * (1.0 - w);
                r = r1 * w + r2 * (1.0 - w);
                result.at<Vec3b>(i, j)[0] = b;
                result.at<Vec3b>(i, j)[1] = g;
                result.at<Vec3b>(i, j)[2] = r;
            }
        }
    }

    imshow("result", result);
    waitKey(0);

    return 0;
}