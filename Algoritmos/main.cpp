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
hashMap matToHash(Mat image, float pPercX, float pPercY,int sampleRate, int pTolerance, float pProbPercentage){
    hashMap hash = hashMap(pPercX, pPercY, pTolerance, pProbPercentage);
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
double compareBT(hashMap hash1,hashMap hash2){
    unsigned t0, t1;
    double time;  
	t0=clock();
	clock_t start, final;
    start = clock();
    int num = 0;
    for(int i=0; i < 256; i++){
        int biggest = hash1.findBiggestBucket();
        num += hash2.getCoincidencesBT(hash1,i);
    }
    t1 = clock();
    time = (double(t1-t0)/CLOCKS_PER_SEC);
    std::cout<<"Coincidencias BT: "<< num <<std::endl;
    return time;
}
double compareDivideAndConquer(hashMap hash1,hashMap hash2){
    unsigned t0, t1;
    double time;  
	t0=clock();
	clock_t start, final;
    start = clock();
    int num = 0;
    for(int i=0; i < 256; i++){
        num += hash2.getCoincidencesDivideAndConquer(hash1,i);
    }
    t1 = clock();
    time = (double(t1-t0)/CLOCKS_PER_SEC);
    std::cout<<"Coincidencias divide and conquer: "<< num <<std::endl;
    return time;
}

double compareProbabilistic(hashMap hash1,hashMap hash2){
    unsigned t0, t1;
    double time;   
	t0=clock();
	clock_t start, final;
    start = clock();
    int num = 0;
    for(int i=0; i < 256; i++){
        num += hash2.getCoincidencesProbabilistic(hash1,i);
    }
    t1 = clock();
    time = (double(t1-t0)/CLOCKS_PER_SEC);
    std::cout<<"Coincidencias probabilistic: "<< num <<std::endl;
    return time;
}

int main(){
    double values[5];
    readFile("values.txt", values,5);
    int index = 0;

    Mat image1 = imread("original.jpg");
    Mat image2 = imread("inserciones.jpg");
    image1 = rescale(image1);
    image2 = rescale(image2);
    hashMap hash1 = matToHash(image1, values[0], values[1], values[2], values[3], values[4]);
    hashMap hash2 = matToHash(image2, values[0], values[1], values[2], values[3], values[4]);
    cout<<"Tiempo transcurrido: " <<compareDivideAndConquer(hash1,hash2)<<" segundos"<<endl;
    cout<<"Tiempo transcurrido: " <<compareBT(hash1,hash2)<<" segundos"<<endl;
    hash1 = matToHash(image1, values[0], values[1], values[2], values[3], values[4]);
    hash2 = matToHash(image2, values[0], values[1], values[2], values[3], values[4]);
    cout<<"Tiempo transcurrido: " <<compareProbabilistic(hash1,hash2)<<" segundos"<<endl;
    return 0;
}
