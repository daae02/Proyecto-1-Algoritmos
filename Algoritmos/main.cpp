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
    for(int y = 0; y+SHEIGHT < yTimes;){
        for(int x = 0; x+SWIDTH < xTimes;){
            hash.insert(copy(image,x,y),x,y);
            x +=sampleRate;
        }
        y+=sampleRate;
    }

    return hash;
}
int compare(hashMap hash1,hashMap hash2){
    int num = 0;
    for(int i=0; i < 256; i++){
        std::cout<<"Bucket # "<<i<<std::endl;
        num = hash2.getCoincidences(hash1,i, num);
    }
    std::cout<<"Coincidencias: "<< num <<std::endl;
    return 0;
}
int main(){
    float values[4];
    readFile("values.txt", values,4);
    int index = 0;
    /*while(index != 4){
		cout << "value: " << values[index] << endl;
		index++;
	}*/
    Mat image1 = imread("imag2.jpg");
    Mat image2 = imread("imag1con2.jpg");
    image1 = rescale(image1);
    image2 = rescale(image2);
    std::cout<<"Termina reescalado"<<std::endl;
    hashMap hash1 = matToHash(image1, values[0], values[1], values[2], values[3]);
    cout << "hice hash1" << endl;
    hashMap hash2 = matToHash(image2, values[0], values[1], values[2], values[3]);
    hashBucket bucket = hash1.buckets[46];
    hashNode * tempNode = bucket.first;
    while(tempNode != bucket.last){
        cout << tempNode->g << endl;
        tempNode = tempNode ->next;
    }
    /*std::cout<<"Termina hashes"<<std::endl;
    int num = compare(hash1,hash2);
    std::cout<<"Termina comparacion"<<std::endl;*/
    imshow("Prueba de que somos un genio", image2);
    waitKey(0);
    return 0;
}
