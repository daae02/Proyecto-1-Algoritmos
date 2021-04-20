#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;

int main(){
    std::cout << "OpenCV Hello the world..." << std::endl;
    Mat image;
    image = imread("fabs.jpeg",1);
    double width = floor(image.cols * 0.5) ;
    double height = floor(image.rows * 0.5);
    Mat image2;
     std::cout <<image.cols<<", "<<image.rows<<std::endl;
    std::cout <<width<<", "<<height<<std::endl;
    Size tam = Size(width,height);
    image2.cols = width;
    image2.rows = height;
    resize(image,image2,image2.size(),0,0,INTER_CUBIC);
    Mat gray;
    cvtColor(image2,gray,COLOR_RGBA2GRAY,100);
    double minVal, maxVal;
    minMaxLoc(gray, &minVal, &maxVal);
    /*gray.convertTo(gray, CV_8U, 10/(maxVal - minVal), -minVal * 250/(maxVal - minVal));  */
    std::vector<uchar> array;
    array.assign(gray.data, gray.data + gray.total()*gray.channels());
    imshow("START PROJECT WITH OPENCV", gray);
    imwrite("test_gray.jpg", gray);
    waitKey(0);
    return 0;
}