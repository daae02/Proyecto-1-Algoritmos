#include <iostream>
#include "structs.cpp"
#define SCALE 0.5 
#define SWIDTH floor(192*SCALE) 
#define SHEIGHT floor(108*SCALE) 
Mat copy(Mat source, int pX, int pY){
    Rect roi(pX,pY,SWIDTH,SHEIGHT);
    Mat sliding = source(roi);
    return sliding;
}
Mat rescale(Mat image){
    int width = floor(image.cols * SCALE) ;
    int height = floor(image.rows * SCALE);
    Mat resized = Mat(height,width,3);
    resize(image,resized,resized.size(),0,0,INTER_CUBIC);
    return resized;
}
hashMap matToHash(Mat image){
    hashMap hash = hashMap();
    int yTimes = image.rows;
    int xTimes = image.cols; 
    for(int y = 0; y+SHEIGHT < yTimes;y++){
        for(int x = 0; x+SWIDTH < xTimes;x++){
            hash.insert(copy(image,x,y));
        }
    }
    return hash;
}
int compare(hashMap hash1,hashMap hash2){
    int num = 0;
    for(int i=0; i < 256; i++){
        std::cout<<"Bucket # "<<i<<std::endl;
        num = hash1.getCoincidences(hash2,i);
        if (num != 0){
            std::cout<<"Coincidencias: "<< num <<std::endl;
            return num;
        }
    }
    std::cout<<"Coincidencias: "<< num <<std::endl;
    return 0;
}
int main(){
    Mat image1 = imread("image1.jpg",1);
    Mat image2 = imread("image2.jpg",1);
    image1 = rescale(image1);
    image2 = rescale(image2);
    std::cout<<"Termina reescalado"<<std::endl;
    hashMap hash1 = matToHash(image1);
    hashMap hash2 = matToHash(image2);
    std::cout<<"Termina hashes"<<std::endl;
    int num = compare(hash1,hash2);
    String name = "sale"+std::to_string(num)+".jpg";
    std::cout<<"Termina comparacion"<<std::endl;
    imwrite(name,image2);
    imshow("Prueba de que soy un genio", image1);
    waitKey(0);
    return 0;
}
