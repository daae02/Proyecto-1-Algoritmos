#include <opencv2/opencv.hpp>
#include "files.cpp"
using namespace cv;
#define SCALE 0.25
#define SWIDTH floor(192*SCALE) 
#define SHEIGHT floor(108*SCALE) 

struct hashBucket;

struct hashNode{
    Mat value;
    int r;
    int g;
    int x;
    int y;
    hashNode* next;
    hashNode* before;
    hashNode* followingMat;
    hashNode* verticalFollowingMat;
    hashBucket * container;
    bool paired;
    hashNode(){}
    hashNode(Mat pValue, int px, int py,hashBucket * pContainer){
        value = pValue;
        r = value.at<cv::Vec3b>(0,0)[0];
        g = value.at<cv::Vec3b>(0,0)[1];
        x = px;
        y = py;
        next = NULL;
        before = NULL;
        paired = false;
        followingMat = NULL;
        verticalFollowingMat = NULL;
        container = pContainer;
    }
};
struct hashBucket{
    bool checked;
    hashNode* first;
    hashNode* last;
    int cant;
    hashBucket(){
        first = last = NULL;
        cant = 0;
        checked = false;
    }
    hashNode* insert(Mat table, int x, int y){
        cant++;
        hashNode* newNode = new hashNode(table,x, y, this);
        if (first == NULL){
            first = last = newNode;
        }
        else{
            if(newNode->g < first->g){
                newNode -> next = first;
                first -> before = newNode;
                first = newNode;
                
            }
            else{
                hashNode*tempNode = first;
                while(tempNode != NULL){
                    if(newNode->g < tempNode->g){
                        tempNode -> before -> next = newNode;
                        newNode ->next = tempNode;
                        newNode ->before = tempNode ->before;
                        tempNode -> before = newNode;
                        return newNode;
                    }
                    tempNode = tempNode ->next;
                }
                newNode ->before = last;
                last -> next = newNode;
                last = newNode;
            }
        }
        return newNode;
    }
};
struct hashMap{
    hashBucket buckets[256];
    double percX;
    double percY;
    int tolerance;
    hashMap(double pPercX, double pPercY, int pTolerance){
        percX = pPercX;
        std::cout << percX << endl; 
        percY = pPercY;
        std::cout << percY << endl;
        tolerance = pTolerance;
        std::cout << pTolerance << endl;
        for(int i=0; i < 256; i++){
            buckets[i] = hashBucket();
        }
    }
    hashNode* insert(Mat table, int x, int y){
        Vec3b pixelColor = table.at<cv::Vec3b>(0,0);
        return buckets[pixelColor[0]].insert(table,x,y);
         
    }
    int findBiggestBucket(){
        hashBucket * biggest = NULL;
        int biggestNumber;
        for (int i = 0; i < 256; i++)
        {
            if((biggest == NULL || biggest->cant<buckets[i].cant) && !buckets[i].checked){
                biggest = &buckets[i];
                biggestNumber = i;
            }
        }
        biggest->checked=true;
        return biggestNumber;
    } 

    void evaluateMats(hashNode * hash, int px, int py, int shifting){
        if(hash != NULL && shifting < SWIDTH &&(hash->y < py || hash->x < px)){
            if (!hash->paired){
                hash->container->cant--;
                hash->paired= true;
            }
            shifting++;
            if (hash->x < px)
                evaluateMats(hash->followingMat, px, py,shifting);
            if (hash->y < py)
                evaluateMats(hash->verticalFollowingMat, px, py,shifting);
        }        
    }

    bool matDifference(hashNode * fuente,hashNode * destino, int count){
        Mat matSrc = fuente->value;
        Mat matDest = destino->value;
        String name1, name2;
        int matY = floor(matSrc.rows*percX);
        int matX = floor(matSrc.cols*percY);
        int total = matSrc.rows * matSrc.cols;
        int cont = 0;
        for (int y = 0; y < matY; y++)
        {
            for (int x = 0; x < matX; x++)
            {
                if (abs(matSrc.at<cv::Vec3b>(y,x)[0] - matDest.at<cv::Vec3b>(y,x)[0]) > tolerance ||
                    abs(matSrc.at<cv::Vec3b>(y,x)[1] - matDest.at<cv::Vec3b>(y,x)[1]) > tolerance ||
                    abs(matSrc.at<cv::Vec3b>(y,x)[2] - matDest.at<cv::Vec3b>(y,x)[2]) > tolerance )
                    {
                        if(x > matX*0.2 && y > matY*0.5)
                            evaluateMats(fuente,fuente->x+x,fuente->y+y, 0);
                        return false;
                    }
                }
            }
        std::cout<<"Coincidencia encontrada"<<std::endl;
        return true;
        
    }
    int getCoincidences(hashMap pHash, int pBucket,int count){
        /*std::cout<<"Pixeles en source: "<< this->buckets[pBucket].cant <<std::endl;
        std::cout<<"Pixeles en dest: "<< pHash.buckets[pBucket].cant <<std::endl;*/
        hashNode* tmpSrc = this->buckets[pBucket].first;
        hashNode* tmpDest;
        while(tmpSrc != NULL){
            tmpDest = pHash.buckets[pBucket].first;
            while(tmpDest!= NULL){
                if(abs(tmpDest->g -tmpSrc->g)< tolerance
                && !tmpDest->paired
                && matDifference(tmpSrc,tmpDest, count)){
                    count++;
                    tmpDest->paired = true;
                    break;
                }
                tmpDest = tmpDest->next;
            }
            tmpSrc = tmpSrc->next;
        }
        return count;
    }
};