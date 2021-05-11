#include <iostream>
#include "structs.cpp"

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
hashMap matToHash(Mat image, float pPercX, float pPercY,int sampleRate, int pTolerance){
    hashMap hash = hashMap(pPercX, pPercY, pTolerance);
    int yTimes = image.rows;
    int xTimes = image.cols;
    std::vector<hashNode*> verticalBeforeNode;
    hashNode * beforeNode;
    hashNode * current = NULL;
    for(int y = 0; y+SHEIGHT < yTimes;){
        beforeNode = NULL;
        for(int x = 0; x+SWIDTH < xTimes;){
            current = hash.insert(copy(image,x,y),x,y);
            if(beforeNode != NULL){
                beforeNode->followingMat = current;
            }
            if(!verticalBeforeNode.empty()){
                verticalBeforeNode.at(verticalBeforeNode.size()-1)->verticalFollowingMat = current;
            }
            verticalBeforeNode.push_back(current);
            x +=sampleRate;
            beforeNode = current;
        }
        verticalBeforeNode.clear();
        y+=sampleRate;
    }

    return hash;
}
int compare(hashMap hash1,hashMap hash2){
    int num = 0;
    for(int i=0; i < 256; i++){
        int biggest = hash1.findBiggestBucket();
        std::cout<<"Bucket # "<<biggest<<std::endl;
        num = hash2.getCoincidences(hash1,i, num);
    }
    std::cout<<"Coincidencias: "<< num <<std::endl;
    return 0;
}
int main(){
    double values[4];
    readFile("values.txt", values,4);
    int index = 0;

    Mat image1 = imread("imag1.jpg");
    Mat image2 = imread("imag2con1.jpg");
    image1 = rescale(image1);
    image2 = rescale(image2);
    std::cout<<"Termina reescalado"<<std::endl;
    hashMap hash1 = matToHash(image1, values[0], values[1], values[2], values[3]);
    cout << "hice hash1" << endl;
    hashMap hash2 = matToHash(image2, values[0], values[1], values[2], values[3]);
    std::cout<<"Termina hashes"<<std::endl;
    int num = compare(hash1,hash2);
    std::cout<<"Termina comparacion"<<std::endl;
    imshow("Prueba de que somos un genio", image2);
    waitKey(0);
    return 0;
}
