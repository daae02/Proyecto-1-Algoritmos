#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
#define SCALE 0.5 
#define SWIDTH floor(192*SCALE) 
#define SHEIGHT floor(108*SCALE) 
Mat copy(Mat source, int pX, int pY){
    Rect roi(pX,pY,SWIDTH,SHEIGHT);
    Mat sliding = source(roi);
    String name = "test"+std::to_string(pX)+"_"+std::to_string(pY)+".jpg";
    return sliding;
}
int main(){
    std::cout << "OpenCV Hello the world..." << std::endl;
    Mat image;
    image = imread("fabs.jpeg",1);
    int width = floor(image.cols * SCALE) ;
    int height = floor(image.rows * SCALE);
    Mat image2 = Mat(height,width,3);
    resize(image,image2,image2.size(),0,0,INTER_CUBIC);
    int xTimes = floor(image2.cols/SWIDTH);
    int yTimes = floor(image2.rows/SHEIGHT);
    for(int y = 0; y < yTimes;y++){
        for(int x = 0; x < xTimes;x++){
            copy(image2,x*SWIDTH,y*SHEIGHT);
        }
    }
    imshow("START PROJECT WITH OPENCV", image2);
    imwrite("test_gray.jpg", image2);
    waitKey(0);
    return 0;
}