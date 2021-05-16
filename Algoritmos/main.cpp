#include <iostream>
#include "structs.cpp"

Mat copy(Mat pSource, int pX, int pY){
    Rect roi(pX,pY,SWIDTH,SHEIGHT);
    Mat sliding = pSource(roi);
    return sliding;
}
Mat rescale(Mat pImage){
    int width = floor(pImage.cols * SCALE) ;
    int height = floor(pImage.rows * SCALE);
    Mat resized = Mat(height,width,3);
    resize(pImage,resized,resized.size(),0,0,INTER_CUBIC);
    return resized;
}
hashMap matToHash(Mat pImage, float pPercX, float pPercY,int pSampleRate, int pTolerance, float pProbPercentage){
    hashMap hash = hashMap(pPercX, pPercY, pTolerance, pProbPercentage);
    int yTimes = pImage.rows;
    int xTimes = pImage.cols;
    std::vector<hashNode*> verticalBeforeNode;
    hashNode * beforeNode;
    hashNode * current = NULL;
    for(int y = 0; y+SHEIGHT < yTimes;){
        beforeNode = NULL;
        for(int x = 0; x+SWIDTH < xTimes;){
            current = hash.insert(copy(pImage,x,y),x,y);
            if(beforeNode != NULL){
                beforeNode->followingMat = current;
            }
            if(!verticalBeforeNode.empty()){
                verticalBeforeNode.at(verticalBeforeNode.size()-1)->verticalFollowingMat = current;
            }
            verticalBeforeNode.push_back(current);
            x +=pSampleRate;
            beforeNode = current;
        }
        verticalBeforeNode.clear();
        y+=pSampleRate;
    }

    return hash;
}
double compareBT(hashMap pHash1,hashMap pHash2){
    unsigned t0, t1;
    double time;  
	t0=clock();
	clock_t start, final;
    start = clock();
    int num = 0;
    for(int bucketNum=0; bucketNum < 256; bucketNum++){
        //cout << "Bucket #" << bucketNum <<endl;
        int biggest = pHash1.findBiggestBucket();
        num += pHash2.getCoincidencesBT(pHash1,bucketNum);
    }
    t1 = clock();
    time = (double(t1-t0)/CLOCKS_PER_SEC);
    //std::cout<<"Coincidencias BT: "<< num <<std::endl;
    return time;
}
double compareDivideAndConquer(hashMap pHash1,hashMap pHash2){
    unsigned t0, t1;
    double time;  
	t0=clock();
	clock_t start, final;
    start = clock();
    int num = 0;
    for(int bucketNum=0; bucketNum < 256; bucketNum++){
        //cout << "Bucket #" << bucketNum <<endl;
        num += pHash2.getCoincidencesDivideAndConquer(pHash1,bucketNum);
    }
    t1 = clock();
    time = (double(t1-t0)/CLOCKS_PER_SEC);
    std::cout<<"Coincidencias divide and conquer: "<< num <<std::endl;
    return time;
}

double compareProbabilistic(hashMap pHash1,hashMap pHash2){
    unsigned t0, t1;
    double time;   
	t0=clock();
	clock_t start, final;
    start = clock();
    int num = 0;
    for(int bucketNum=0; bucketNum < 256; bucketNum++){
        //cout << "Bucket #" << bucketNum <<endl;
        num += pHash2.getCoincidencesProbabilistic(pHash1,bucketNum);
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
    // --- Generación de la estructura de datos ---
    Mat image1 = imread("img3.jpg");
    Mat image2 = imread("img2.jpg");
    image1 = rescale(image1);
    image2 = rescale(image2);
    hashMap hash1 = matToHash(image1, values[0], values[1], values[2], values[3], values[4]);
    hashMap hash2 = matToHash(image2, values[0], values[1], values[2], values[3], values[4]);

    // --- Llamada Divide and Conquer ---
    cout<<"Tiempo transcurrido: " <<compareDivideAndConquer(hash1,hash2)<<" segundos"<<endl;
    
    // --- Llamada Backtracking con Dinámica ---
    //cout<<"Tiempo transcurrido: " <<compareBT(hash1,hash2)<<" segundos"<<endl;
    
    // --- Regeneración de datos ---
    hash1 = matToHash(image1, values[0], values[1], values[2], values[3], values[4]);
    hash2 = matToHash(image2, values[0], values[1], values[2], values[3], values[4]);
    
    // --- Llamada Probabilístico ---
    cout<<"Tiempo transcurrido: " <<compareProbabilistic(hash1,hash2)<<" segundos"<<endl;
    return 0;
}
