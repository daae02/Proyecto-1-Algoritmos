#include <opencv2/opencv.hpp>
#include "files.cpp"
using namespace cv;
#define SCALE 0.5
#define SWIDTH floor(192*SCALE) 
#define SHEIGHT floor(108*SCALE) 



struct hashNode{
    Mat value;
    int r;
    int g;
    int x;
    int y;
    hashNode* next;
    hashNode* before;
    bool paired;
    hashNode(Mat pValue, int px, int py){
        value = pValue;
        r = value.at<cv::Vec3b>(0,0)[0];
        g = value.at<cv::Vec3b>(0,0)[1];
        x = px;
        y = py;
        next = NULL;
        before = NULL;
        paired = false;
    }
};
struct hashBucket{
    hashNode* first;
    hashNode* last;
    int cant;
    hashBucket(){
        first = last = NULL;
        cant = 0;
    }
    void insert(Mat table, int x, int y){
        cant++;
        hashNode* newNode = new hashNode(table,x, y);
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
                        return;
                    }
                    tempNode = tempNode ->next;
                }
                newNode ->before = last;
                last -> next = newNode;
                last = newNode;
            }
        }
        
    }
};
struct hashMap{
    hashBucket buckets[256];
    float percX;
    float percY;
    int tolerance;
    hashMap(float pPercX, float pPercY, int pTolerance){
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
    void insert(Mat table, int x, int y){
        Vec3b pixelColor = table.at<cv::Vec3b>(0,0);
        buckets[pixelColor[0]].insert(table,x,y);
    }
    bool matXor(hashNode * fuente,hashNode * destino, int count){
        Mat matSrc = fuente->value;
        Mat matDest = destino->value;
        String name1, name2;
        int matY = floor(matSrc.rows*percX);
        int matX = floor(matSrc.cols*percY);
        //int tolerace = floor((matY*matX)*0.60);
        for (int y = 0; y < matY; y++)
        {
            for (int x = 0; x < matX; x++)
            {
                if (abs(matSrc.at<cv::Vec3b>(y,x)[0] - matDest.at<cv::Vec3b>(y,x)[0]) > tolerance ||
                    abs(matSrc.at<cv::Vec3b>(y,x)[1] - matDest.at<cv::Vec3b>(y,x)[1]) > tolerance ||
                    abs(matSrc.at<cv::Vec3b>(y,x)[2] - matDest.at<cv::Vec3b>(y,x)[2]) > tolerance )
                    {
                            return false;
   
                        //}
                    }
                }
            }
        
        name1 = "coincs/"+std::to_string(count)+"_"+std::to_string(fuente->x)+"_"+std::to_string(fuente->y)+"n1.jpg";
        name2 = "coincs/"+std::to_string(count)+"_"+std::to_string(destino->x)+"_"+std::to_string(destino->y)+"n2.jpg";
        imwrite(name1,matSrc);
        imwrite(name2,matDest);
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
                && matXor(tmpSrc,tmpDest, count)){
                    count++;
                    tmpDest->paired = true;
                    break;
                }
                tmpDest = tmpDest->next;
            }
            /*if(count != 0){
                return count;        
            }*/
            tmpSrc = tmpSrc->next;
        }
        return count;
    }
};